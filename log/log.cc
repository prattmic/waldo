#include <memory>
#include <utility>
#include "io/byteio.h"
#include "log/log.h"

namespace logging {

namespace internal {

std::unique_ptr<io::ByteIO> sink;

const char *levels[] = {
    [INFO] = "INFO",
    [WARNING] = "WARNING",
    [ERROR] = "ERROR",
};

}  // namespace internal

void SetupLogger(std::unique_ptr<io::ByteIO> io) {
    logging::internal::sink = std::move(io);
}

}  // namespace logging
