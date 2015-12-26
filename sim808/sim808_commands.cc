#include <chrono>
#include "external/nanopb/util/task/status.h"
#include "external/nanopb/util/task/statusor.h"
#include "sim808/sim808.h"

namespace sim808 {

StatusOr<bool> SIM808::GNSEnabled() {
    io_->FlushRead();

    char response[2] = { '\0' };
    auto statusor = SendSynchronousCommand("AT+CGNSPWR?", "CGNSPWR",
                                           response, 2,
                                           std::chrono::milliseconds(100));
    if (!statusor.ok())
        return statusor.status();

    if (statusor.Value() != 1)
        return Status(::util::error::Code::UNKNOWN, "wrong size response");

    if (response[0] == '0')
        return false;

    return true;
}

}  // namespace sim808
