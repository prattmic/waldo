#ifndef IO_LOGGING_BYTEIO_H_
#define IO_LOGGING_BYTEIO_H_

#include <ctype.h>
#include <string.h>
#include <memory>
#include <utility>
#include "external/nanopb/util/task/status.h"
#include "external/nanopb/util/task/statusor.h"
#include "io/byteio.h"
#include "log/log.h"
#include "util/string.h"

namespace io {

namespace {

// Create a nice readable string for a char.
// Biggest possible is "0xFF\0" -> 5 bytes
void pretty_char(char c, char buf[5]) {
    memset(buf, 0, 5);

    if (isprint(c)) {
        buf[0] = '\'';
        buf[1] = c;
        buf[2] = '\'';
        buf[4] = '\0';
    } else if (c == '\r') {
        memcpy(buf, "'\\r'", 6);
    } else if (c == '\n') {
        memcpy(buf, "'\\n'", 6);
    } else {
        buf[0] = '0';
        buf[1] = 'x';
        ::util::uitoa(c, &buf[2], 4, 16);
    }
}

}  // namespace

// Logs reads and writes to a logger ByteIO.
class LoggingByteIO : public ByteIO {
 public:
    LoggingByteIO(std::unique_ptr<ByteIO> io) : io_(std::move(io)) {}

    // Read a single byte.
    virtual ::util::StatusOr<char> Read() override {
        auto statusor = io_->Read();
        if (statusor.ok()) {
            char buf[5];
            pretty_char(statusor.Value(), buf);

            LOG(INFO) << "Read: " << buf;
        } else {
            LOG(INFO) << "Error reading: "
                      << statusor.status().error_message();
        }

        return statusor;
    }

    // Write a single byte.
    virtual ::util::Status Write(char c) override {
        char buf[5];
        pretty_char(c, buf);

        LOG(INFO) << "Writing: " << buf;

        auto status = io_->Write(c);
        if (!status.ok()) {
            LOG(INFO) << "Error writing: " << status.error_message();
        }

        return status;
    }

    // For StatusOr.
    LoggingByteIO() : io_(nullptr) {}

    // Move constructor.
    LoggingByteIO(LoggingByteIO&& other)
        : io_(std::move(other.io_)) {
        other.io_ = nullptr;
    }

    // Copy constructor.
    LoggingByteIO(const LoggingByteIO& other) = delete;

 private:
    std::unique_ptr<io::ByteIO> io_;
};

}  // namespace io

#endif  // IO_LOGGING_BYTEIO_H_
