#ifndef IO_BYTEIO_H_
#define IO_BYTEIO_H_

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
};

}  // namespace io

#endif  // IO_BYTEIO_H_
