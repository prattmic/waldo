#include <chrono>
#include "external/nanopb/util/task/status.h"
#include "external/nanopb/util/task/statusor.h"
#include "sim808/sim808.h"

namespace sim808 {

Status SIM808::VerifyResponse(const char *expected,
                              std::chrono::system_clock::time_point timeout) {
    while (*expected) {
        if (std::chrono::system_clock::now() > timeout)
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
        auto end = std::chrono::system_clock::now()
            + std::chrono::milliseconds(100);

        auto status = io_->FlushRead();
        if (!status.ok())
            return status;

        auto statusor = io_->WriteString("AT\r", end);
        if (!statusor.ok()) {
            TryAbort();
            return statusor.status();
        }

        status = VerifyResponse("AT\r\r\nOK\r\n", end);
        if (status.ok())
            return Status::OK;
    }

    return Status(::util::error::Code::DEADLINE_EXCEEDED,
                  "unable to initialize device auto-baud");
}

// Disabling command echoing makes further command responses easier to handle.
// We can't use SendSimpleCommand because it assumes that command echoing is
// disabled.
Status SIM808::DisableCommandEcho() {
        auto end = std::chrono::system_clock::now()
            + std::chrono::milliseconds(100);

        auto statusor = io_->WriteString("ATE0\r", end);
        if (!statusor.ok()) {
            TryAbort();
            return statusor.status();
        }

        return VerifyResponse("ATE0\r\r\nOK\r\n", end);
}

// Check if the device is ready and can skip initialization.
Status SIM808::CheckReady() {
    return SendSimpleCommand("AT", "OK", std::chrono::milliseconds(100));
}

Status SIM808::Initialize() {
    auto status = io_->FlushRead();
    if (!status.ok())
        return status;

    status = CheckReady();
    if (status.ok())
        return Status::OK;

    status = InitAutoBaud();
    if (!status.ok())
        return status;

    return DisableCommandEcho();
}

Status SIM808::WriteCommand(const char *command,
                            std::chrono::system_clock::time_point timeout) {
    auto statusor = io_->WriteString(command, timeout);
    if (!statusor.ok()) {
        return statusor.status();
    }

    while (1) {
        if (std::chrono::system_clock::now() > timeout)
            return Status(::util::error::Code::DEADLINE_EXCEEDED, "timeout");

        auto status = io_->Write('\r');
        if (!status.ok()) {
            // Would block. retry.
            if (status.error_code() ==
                ::util::error::Code::RESOURCE_EXHAUSTED)
                continue;

            return status;
        }

        break;
    }

    return Status::OK;
}

Status SIM808::SendSimpleCommand(const char *command, const char *response,
                                 std::chrono::milliseconds timeout) {
    auto end = std::chrono::system_clock::now() + timeout;

    auto status = WriteCommand(command, end);
    if (!status.ok()) {
        TryAbort();
        return status;
    }

    // Preamble
    status = VerifyResponse("\r\n", end);
    if (!status.ok())
        return status;

    status = VerifyResponse(response, end);
    if (!status.ok())
        return status;

    // Termination
    return VerifyResponse("\r\n", end);
}

StatusOr<size_t> SIM808::SendSynchronousCommand(
        const char *command, const char *prefix, char *response,
        size_t size, std::chrono::milliseconds timeout) {
    auto end = std::chrono::system_clock::now() + timeout;

    auto status = WriteCommand(command, end);
    if (!status.ok()) {
        TryAbort();
        return status;
    }

    // Preamble
    status = VerifyResponse("\r\n", end);
    if (!status.ok())
        return status;

    // Synchronous response start
    status = VerifyResponse("+", end);
    if (!status.ok())
        return status;

    status = VerifyResponse(prefix, end);
    if (!status.ok())
        return status;

    status = VerifyResponse(": ", end);
    if (!status.ok())
        return status;

    // Response content
    size_t total = 0;
    while (1) {
        auto statusor = io_->Read();
        if (!statusor.ok()) {
            // Would block. retry.
            if (statusor.status().error_code() !=
                ::util::error::Code::RESOURCE_EXHAUSTED)
                continue;

            return status;
        }

        char c = statusor.Value();

        // End of the response?
        if (c == '\r')
            break;

        // We keep consuming the response even if there is no more room
        // in the buffer, so we can see if the command itself was successful.
        if (size > 0) {
            *response++ = c;
            total++;
            size--;
        }
    }

    // Everything is A-OK?
    status = VerifyResponse("\n\r\nOK\r\n", end);
    if (!status.ok())
        return status;

    return total;
}

}  // namespace sim808
