#ifndef IO_LOGGING_BYTEIO_H_
#define IO_LOGGING_BYTEIO_H_

#include <ctype.h>
#include <string.h>
#include <memory>
#include <utility>
#include "external/nanopb/util/task/status.h"
#include "external/nanopb/util/task/statusor.h"
#include "io/byteio.h"
#include "util/string.h"

namespace io {

namespace {

// Create a nice readable string for a char.
// Biggest possible is "0xFF\n\0" -> 6 bytes
void pretty_char(char c, char buf[6]) {
    memset(buf, 0, 6);

    if (isprint(c)) {
        buf[0] = '\'';
        buf[1] = c;
        buf[2] = '\'';
        buf[3] = '\n';
        buf[4] = '\0';
    } else if (c == '\r') {
        memcpy(buf, "'\\r'\n", 6);
    } else if (c == '\n') {
        memcpy(buf, "'\\n'\n", 6);
    } else {
        buf[0] = '0';
        buf[1] = 'x';
        ::util::uitoa(c, &buf[2], 4, 16);
        if (buf[3] == '\0')
            buf[3] = '\n';
        else
            buf[4] = '\n';
    }
}

}  // namespace

// Logs reads and writes to a logger ByteIO. Logging is best-effort, no
// error checking is done.
class LoggingByteIO : public ByteIO {
 public:
    LoggingByteIO(std::unique_ptr<ByteIO> io, std::unique_ptr<ByteIO> log)
        : io_(std::move(io)),
          log_(std::move(log)) {}

    // Read a single byte.
    virtual ::util::StatusOr<char> Read() override {
        auto statusor = io_->Read();
        if (statusor.ok()) {
            log_->WriteString("Read: ");

            char buf[6];
            pretty_char(statusor.Value(), buf);
            log_->WriteString(buf);
        } else {
            log_->WriteString("Error reading: ");
            log_->WriteString(statusor.status().error_message());
            log_->Write('\n');
        }

        return statusor;
    }

    // Write a single byte.
    virtual ::util::Status Write(char c) override {
        log_->WriteString("Writing: ");

        char buf[6];
        pretty_char(c, buf);
        log_->WriteString(buf);

        auto status = io_->Write(c);
        if (!status.ok()) {
            log_->WriteString("Error writing: ");
            log_->WriteString(status.error_message());
            log_->Write('\n');
        }

        return status;
    }

    // For StatusOr.
    LoggingByteIO() : io_(nullptr), log_(nullptr) {}

    // Move constructor.
    LoggingByteIO(LoggingByteIO&& other)
        : io_(std::move(other.io_)),
          log_(std::move(other.log_)) {
        other.io_ = nullptr;
        other.log_ = nullptr;
    }

    // Copy constructor.
    LoggingByteIO(const LoggingByteIO& other) = delete;

 private:
    std::unique_ptr<io::ByteIO> io_;
    std::unique_ptr<io::ByteIO> log_;
};

}  // namespace io

#endif  // IO_LOGGING_BYTEIO_H_
