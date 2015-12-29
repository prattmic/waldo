#ifndef IO_NULL_BYTEIO_H_
#define IO_NULL_BYTEIO_H_

#include "external/nanopb/util/task/status.h"
#include "external/nanopb/util/task/statusor.h"
#include "io/byteio.h"

namespace io {

class NullByteIO : public ByteIO {
 public:
    NullByteIO() {}

    // Read a single byte.
    virtual ::util::StatusOr<char> Read() override {
        return '\0';
    }

    // Write a single byte.
    virtual ::util::Status Write(char c) override {
        return ::util::Status::OK;
    }

    // Move constructor.
    NullByteIO(NullByteIO&& other) {}
};

}  // namespace io

#endif  // IO_NULL_BYTEIO_H_
