#ifndef IO_BYTEIO_H_
#define IO_BYTEIO_H_

#include <stdint.h>
#include "external/nanopb/util/task/status.h"
#include "external/nanopb/util/task/statusor.h"

namespace io {

class ByteIO {
 public:
    virtual ~ByteIO() = default;

    // Read a single byte.
    virtual ::util::StatusOr<char> Read() = 0;

    // Write a single byte.
    virtual ::util::Status Write(char c) = 0;

    // Write a string. Returns the number of bytes written, or an error
    // if no bytes were written.
    ::util::StatusOr<size_t> WriteString(const char *s) {
        ::util::Status status;
        size_t count = 0;

        while (*s) {
            status = Write(*s);
            if (!status.ok()) {
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
