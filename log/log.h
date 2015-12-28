#ifndef LOG_LOG_H_
#define LOG_LOG_H_

#include <stdint.h>
#include <memory>
#include <utility>
#include "io/byteio.h"
#include "util/string.h"

enum LogLevel {
    INFO,
    WARNING,
    ERROR,
};

namespace logging {

namespace internal {

extern std::unique_ptr<io::ByteIO> sink;
extern const char *levels[];

}  // namespace internal

class Logger {
 public:
    Logger(LogLevel l) {
        if (internal::sink)
            *internal::sink << internal::levels[l] << ": ";
    }

    ~Logger() {
        if (internal::sink)
            *internal::sink << "\n";
    }

    Logger& operator<<(const char *s) {
        if (internal::sink)
            *internal::sink << s;
        return *this;
    }

    Logger& operator<<(bool b) {
        if (internal::sink) {
            if (b)
                *internal::sink << "true";
            else
                *internal::sink << "false";
        }
        return *this;
    }

    Logger& operator<<(uint32_t i) {
        if (internal::sink) {
            // Biggest uint32_t is 10 characters (base 10).
            char buf[11];
            ::util::uitoa(i, buf, 11, 10);

            *internal::sink << buf;
        }
        return *this;
    }

    Logger& operator<<(int32_t i) {
        if (internal::sink) {
            // Biggest int32_t is 11 characters (base 10).
            char buf[12];
            ::util::itoa(i, buf, 12, 10);

            *internal::sink << buf;
        }
        return *this;
    }
};

extern void SetupLogger(std::unique_ptr<io::ByteIO> io);

}  // namespace logging

inline logging::Logger LOG(LogLevel l) {
    return logging::Logger(l);
}

#endif  // LOG_LOG_H_
