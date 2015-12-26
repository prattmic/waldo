#ifndef IO_LOGGING_BYTEIO_H_
#define IO_LOGGING_BYTEIO_H_

#include <memory>
#include <utility>
#include "external/nanopb/util/task/status.h"
#include "external/nanopb/util/task/statusor.h"
#include "io/byteio.h"

namespace io {

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
            char str[3] = { '\0', '\n', '\0' };
            str[0] = statusor.Value();
            log_->WriteString(str);
        } else {
            log_->WriteString("Error reading: ");
            log_->WriteString(statusor.status().error_message());
            log_->Write('\n');
        }

        return statusor;
    }

    // Write a single byte.
    virtual ::util::Status Write(char c) override {
        char str[3] = { c, '\n', '\0' };
        log_->WriteString("Writing: ");
        log_->WriteString(str);

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
