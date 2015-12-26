#include "external/nanopb/util/task/status.h"
#include "external/nanopb/util/task/statusor.h"
#include "sim808/sim808.h"

namespace sim808 {

Status SIM808::VerifyResponse(const char *expected) {
    while (*expected) {
        auto statusor = io_->Read();
        if (!statusor.ok())
            return statusor.status();

        if (statusor.Value() != *expected)
            return Status(::util::error::Code::UNKNOWN, "unexpected response");

        expected++;
    }

    return Status::OK;
}

Status SIM808::Initialize() {
    // TODO(prattmic): We should check that all the bytes were written.
    auto statusor = io_->WriteString("AT\r");
    if (!statusor.ok())
        return statusor.status();

    return VerifyResponse("AT\r\r\nOK\r\n");
}

}  // namespace sim808
