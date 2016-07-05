#ifndef LOG_LOG_H_
#define LOG_LOG_H_

#include <stdint.h>
#include <sys/time.h>
#include <chrono>
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

template<typename Duration>
struct timeval to_timeval(Duration const& d) {
    std::chrono::seconds const sec = std::chrono::duration_cast<std::chrono::seconds>(d);

    struct timeval tv;
    tv.tv_sec  = sec.count();
    tv.tv_usec = std::chrono::duration_cast<std::chrono::microseconds>(d - sec).count();
    return tv;
}

}  // namespace internal

// Pass to Logger stream to set string padding.
typedef int LogPad;

class Logger {
 public:
    ~Logger() {
        if (internal::sink)
            *internal::sink << "\r\n";
    }

    Logger& operator<<(LogPad p) {
        pad_ = p;
        return *this;
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
            ::util::uitoa(i, buf, 11, 10, pad_);

            *internal::sink << buf;
        }
        return *this;
    }

    Logger& operator<<(int32_t i) {
        if (internal::sink) {
            // Biggest int32_t is 11 characters (base 10).
            char buf[12];
            ::util::itoa(i, buf, 12, 10, pad_);

            *internal::sink << buf;
        }
        return *this;
    }

    Logger& operator<<(size_t i) {
        if (internal::sink) {
            // Biggest (64-bit) size_t is 20 characters (base 10).
            char buf[21];
            ::util::uitoa(i, buf, 21, 10, pad_);

            *internal::sink << buf;
        }
        return *this;
    }

    Logger& operator<<(uint16_t i) {
        *this << static_cast<uint32_t>(i);
        return *this;
    }

    Logger& operator<<(int16_t i) {
        *this << static_cast<int32_t>(i);
        return *this;
    }

    Logger& operator<<(std::chrono::system_clock::duration d) {
        struct timeval tv = internal::to_timeval(d);
        *this << tv.tv_sec << "." << LogPad(3) << tv.tv_usec / 1000
              << LogPad(0);
        return *this;
    }

 private:
    LogPad pad_ = 0;
};

extern void SetupLogger(std::unique_ptr<io::ByteIO> io);

}  // namespace logging

inline logging::Logger LOG(LogLevel l) {
    auto log = logging::Logger();
    auto now = std::chrono::system_clock::now();
    log << "[" << now.time_since_epoch() << "] "
        << logging::internal::levels[l] << ": ";
    return log;
}


#endif  // LOG_LOG_H_
