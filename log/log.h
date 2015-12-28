#ifndef LOG_LOG_H_
#define LOG_LOG_H_

#include <memory>
#include <utility>
#include "io/byteio.h"

enum LogLevel {
    INFO,
    WARNING,
    ERROR,
};

namespace logging {

class Logger {
 public:
    Logger() : io_(nullptr) {}

    Logger(std::unique_ptr<io::ByteIO> io) : io_(std::move(io)) {}

    void setup(std::unique_ptr<io::ByteIO> io) {
        io_ = std::move(io);
    }

    template <typename T>
    Logger& operator<<(T v) {
        if (io_ != nullptr)
            *io_ << v;
        return *this;
    }

 private:
    std::unique_ptr<io::ByteIO> io_;
};

extern void SetupLogger(std::unique_ptr<io::ByteIO> io);

}  // namespace logging

extern logging::Logger& LOG(LogLevel l);

#endif  // LOG_LOG_H_
