#ifndef IO_BYTEIO_H_
#define IO_BYTEIO_H_

#include <stdint.h>
#include "external/nanopb/util/task/status.h"
#include "external/nanopb/util/task/statusor.h"

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
    // if no bytes were written. Blocks until all bytes are written.
    ::util::StatusOr<size_t> WriteString(const char *s) {
        ::util::Status status;
        size_t count = 0;

        while (*s) {
            status = Write(*s);
            if (!status.ok()) {
                // Would block. retry.
                if (status.error_code() !=
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
    };
};

}  // namespace io

#endif  // IO_BYTEIO_H_
