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

    template <typename T>
    Logger& operator<<(T v) {
        if (internal::sink)
            *internal::sink << v;
        return *this;
    }
};

extern void SetupLogger(std::unique_ptr<io::ByteIO> io);

}  // namespace logging

inline logging::Logger LOG(LogLevel l) {
    return logging::Logger(l);
}

#endif  // LOG_LOG_H_
