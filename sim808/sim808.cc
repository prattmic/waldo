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

// The SIM808 turns on in auto-baud mode. We need to send "AT" a few times
// to get it ready for further commands.
Status SIM808::InitAutoBaud() {
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
                  "unable to initialize device auto-baud");
}

// Disabling command echoing makes further command responses easier to handle.
// We can't use SendCommand because it assumes that command echoing is
// disabled.
Status SIM808::DisableCommandEcho() {
        auto statusor = io_->WriteString("ATE0\r");
        if (!statusor.ok()) {
            TryAbort();
            return statusor.status();
        }

        return VerifyResponse("ATE0\r\r\nOK\r\n",
                              std::chrono::milliseconds(100));
}

// Check if the device is ready and can skip initialization.
Status SIM808::CheckReady() {
    return SendCommand("AT", "OK", std::chrono::milliseconds(100));
}

Status SIM808::Initialize() {
    auto status = CheckReady();
    if (status.ok())
        return Status::OK;

    status = InitAutoBaud();
    if (!status.ok())
        return status;

    return DisableCommandEcho();
}

Status SIM808::SendCommand(const char *command, const char *response,
                           std::chrono::milliseconds timeout) {
    auto statusor = io_->WriteString(command);
    if (!statusor.ok()) {
        TryAbort();
        return statusor.status();
    }

    while (1) {
        auto status = io_->Write('\r');
        if (!status.ok()) {
            // Would block. retry.
            if (status.error_code() !=
                ::util::error::Code::RESOURCE_EXHAUSTED)
                continue;

            return status;
        }

        break;
    }

    // Preamble
    auto status = VerifyResponse("\r\n", timeout);
    if (!status.ok())
        return status;

    // TODO: don't pass the same timeout multiple times.
    status = VerifyResponse(response, timeout);
    if (!status.ok())
        return status;

    // Termination
    return VerifyResponse("\r\n", timeout);
}

}  // namespace sim808
