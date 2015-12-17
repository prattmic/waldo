#ifndef IO_LINUX_BYTEIO_H_
#define IO_LINUX_BYTEIO_H_

#include <unistd.h>
#include "external/nanopb/util/task/status.h"
#include "external/nanopb/util/task/statusor.h"
#include "io/byteio.h"

namespace io {

class LinuxByteIO : ByteIO {
 public:
    static ::util::StatusOr<LinuxByteIO> OpenFile(const char *filename);

    virtual ~LinuxByteIO() override {
        if (fd_ >= 0) {
            close(fd_);
        }
    }

    // Read a single byte.
    virtual ::util::StatusOr<char> Read() override;

    // Write a single byte.
    virtual ::util::Status Write(char c) override;

    // For StatusOr.
    LinuxByteIO() : fd_(-1) {};

    // Move constructor.
    LinuxByteIO(LinuxByteIO&& other) : fd_(other.fd_) {
        other.fd_ = -1;
    }

    // Copy constructor.
    LinuxByteIO(const LinuxByteIO& other) = delete;

 private:
    LinuxByteIO(int fd) : fd_(fd) {};

    int fd_;
};

}  // namespace io

#endif  // IO_LINUX_BYTEIO_H_