#include <chrono>
#include "external/nanopb/util/task/status.h"
#include "external/nanopb/util/task/statusor.h"
#include "sim808/sim808.h"

namespace sim808 {

Status SIM808::VerifyResponse(const char *expected,
                              std::chrono::milliseconds timeout) {
    auto end = std::chrono::system_clock::now() + timeout;

    while (*expected) {
        if (std::chrono::system_clock::now() > end)
            return Status(::util::error::Code::DEADLINE_EXCEEDED, "timeout");

        auto statusor = io_->Read();
        if (!statusor.ok()) {
            // Would block. retry.
            if (statusor.status().error_code() ==
                ::util::error::Code::RESOURCE_EXHAUSTED)
                continue;

            return statusor.status();
        }

        if (statusor.Value() != *expected)
            return Status(::util::error::Code::UNKNOWN, "unexpected response");

        expected++;
    }

    return Status::OK;
}

void SIM808::TryAbort() {
    // Send a ~ since it is not a valid component of any command,
    // hopefully making the partial command an error.
    io_->WriteString("~\r");
}

Status SIM808::Initialize() {
    for (int i = 0; i < 10; i++) {
        auto status = io_->FlushRead();
        if (!status.ok())
            return status;

        auto statusor = io_->WriteString("AT\r");
        if (!statusor.ok()) {
            TryAbort();
            return statusor.status();
        }

        status = VerifyResponse("AT\r\r\nOK\r\n",
                                std::chrono::milliseconds(100));
        if (status.ok())
            return Status::OK;
    }

    return Status(::util::error::Code::DEADLINE_EXCEEDED,
                  "unable to initialize");
}

}  // namespace sim808
