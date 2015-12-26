#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <utility>
#include "external/nanopb/util/task/status.h"
#include "external/nanopb/util/task/statusor.h"
#include "io/linux_byteio.h"

using io::LinuxByteIO;

util::StatusOr<LinuxByteIO> LinuxByteIO::OpenFile(const char *filename) {
    int fd = open(filename, O_RDWR|O_NONBLOCK);
    if (fd < 0) {
        return util::Status(util::error::Code::UNKNOWN, "open failed");
    }

    return LinuxByteIO(fd);
}

util::StatusOr<char> LinuxByteIO::Read() {
    while (1) {
        char c;
        int n = read(fd_, &c, 1);
        if (n < 0) {
            if (errno == EINTR) {
                continue;
            } else if (errno == EAGAIN) {
                return util::Status(util::error::Code::RESOURCE_EXHAUSTED,
                                    "would block");
            }
            return util::Status(util::error::Code::UNKNOWN, "read failed");
        } else if (n == 0) {
            return util::Status(util::error::Code::RESOURCE_EXHAUSTED, "EOF");
        }

        return c;
    }
}

util::Status LinuxByteIO::Write(char c) {
    while (1) {
        int n = write(fd_, &c, 1);
        if (n < 0) {
            if (errno == EINTR) {
                continue;
            } else if (errno == EAGAIN) {
                return util::Status(util::error::Code::RESOURCE_EXHAUSTED,
                                    "would block");
            }
            return util::Status(util::error::Code::UNKNOWN, "write failed");
        }

        return util::Status::OK;
    }
}
