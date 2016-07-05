#ifndef IO_BYTEIO_H_
#define IO_BYTEIO_H_

#include <stdint.h>
#include <chrono>
#include "external/com_github_prattmic_nanopb/util/task/status.h"
#include "external/com_github_prattmic_nanopb/util/task/statusor.h"

namespace io {

class ByteIO {
 public:
    virtual ~ByteIO() = default;

    // Read a single byte. Returns RESOURCE_EXHAUSTED if no bytes are
    // available.
    virtual ::util::StatusOr<char> Read() = 0;

    // Write a single byte. Returns RESOURCE_EXHAUSTED if no space is
    // available.
    virtual ::util::Status Write(char c) = 0;

    // Flush any read buffer. Consumes all bytes available for reading.
    ::util::Status FlushRead() {
        ::util::StatusOr<char> statusor;

        do {
            statusor = Read();
        } while (statusor.ok());

        if (statusor.status().error_code() ==
                ::util::error::Code::RESOURCE_EXHAUSTED)
            return ::util::Status::OK;

        return statusor.status();
    }

    // Write a string. Returns the number of bytes written, or an error
    // if no bytes were written. Blocks until all bytes are written, or
    // timeout is reached.
    ::util::StatusOr<size_t> WriteString(const char *s,
            std::chrono::system_clock::time_point timeout) {
        ::util::Status status;
        size_t count = 0;

        while (*s) {
            if (std::chrono::system_clock::now() > timeout) {
                if (count)
                    return count;
                else
                    return ::util::Status(
                        ::util::error::Code::DEADLINE_EXCEEDED, "timeout");
            }

            status = Write(*s);
            if (!status.ok()) {
                // Would block. retry.
                if (status.error_code() ==
                    ::util::error::Code::RESOURCE_EXHAUSTED)
                    continue;

                if (count)
                    return count;
                else
                    return status;
            }

            count++;
            s++;
        }

        return count;
    }

    // WriteString with a default timeout of 100ms.
    ::util::StatusOr<size_t> WriteString(const char *s) {
        auto end = std::chrono::system_clock::now()
            + std::chrono::milliseconds(100);

        return WriteString(s, end);
    }

    ByteIO& operator<<(const char *s) {
        WriteString(s);
        return *this;
    }
};

}  // namespace io

#endif  // IO_BYTEIO_H_
