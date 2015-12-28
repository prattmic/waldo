#include <memory>
#include <utility>
#include "io/byteio.h"
#include "log/log.h"

namespace logging {

static Logger global_logger;

void SetupLogger(std::unique_ptr<io::ByteIO> io) {
    global_logger.setup(std::move(io));
}

}  // namespace logging

static const char *level_str[] = {
    [INFO] = "INFO",
    [WARNING] = "WARNING",
    [ERROR] = "ERROR",
};

logging::Logger& LOG(LogLevel l) {
    logging::global_logger << level_str[l] << ": ";

    return logging::global_logger;
}
