#include <chrono>
#include "external/com_github_prattmic_nanopb/util/task/status.h"
#include "external/com_github_prattmic_nanopb/util/task/statusor.h"
#include "log/log.h"
#include "sim808/sim808.h"

namespace sim808 {

void SIM808::TryConsumeLine(std::chrono::system_clock::time_point timeout) {
    while (std::chrono::system_clock::now() < timeout) {
        auto statusor = io_->Read();
        if (!statusor.ok()) {
            // Would block. retry.
            if (statusor.status().error_code() ==
                ::util::error::Code::RESOURCE_EXHAUSTED)
                continue;

            return;
        }

        if (statusor.Value() == '\n')
            return;
    }
}

Status SIM808::VerifyResponse(const char *expected,
                              std::chrono::system_clock::time_point timeout) {
    //LOG(INFO) << "Expecting response: " << expected;

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

        if (statusor.Value() != *expected) {
            // Try to consume the rest of this line (probably ERROR), so the
            // next reader doesn't choke on it.
            if (statusor.Value() != '\n')
                TryConsumeLine(timeout);
            return Status(::util::error::Code::UNKNOWN, "unexpected response");
        }

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
    for (int i = 0; i < 50; i++) {
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
    auto ret = SendSimpleCommand("AT", "OK", std::chrono::milliseconds(100));

    auto end = std::chrono::system_clock::now()
        + std::chrono::milliseconds(100);
    TryConsumeLine(end);

    return ret;
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

Status SIM808::WriteParameterizedCommand(
        const char *command, char delimiter, const char *param,
        std::chrono::system_clock::time_point timeout) {
    if ((delimiter && !param) || (!delimiter && param)) {
        return Status(::util::error::Code::FAILED_PRECONDITION,
                      "none or both of delimiter and param must be set");
    }

    if (!delimiter) {
        // No parameter, just write the whole thing.
        auto statusor = io_->WriteString(command, timeout);
        if (!statusor.ok())
            return statusor.status();

        statusor = io_->WriteString("\r", timeout);
        return statusor.status();
    }

    // Parameter, write out byte-by-byte until we get to the delimiter.
    const char *s = command;
    while (*s) {
        if (std::chrono::system_clock::now() > timeout)
            return Status(::util::error::Code::DEADLINE_EXCEEDED, "timeout");

        if (*s == delimiter) {
            // Found the delimiter, write out the param instead.
            auto statusor = io_->WriteString(param, timeout);
            if (!statusor.ok())
                return statusor.status();
        } else {
            auto status = io_->Write(*s);
            if (!status.ok()) {
                // Would block. retry.
                if (status.error_code() ==
                        ::util::error::Code::RESOURCE_EXHAUSTED)
                    continue;

                return status;
            }
        }

        s++;
    }

    auto statusor = io_->WriteString("\r", timeout);
    return statusor.status();
}

Status SIM808::WriteCommand(const char *command,
                            std::chrono::system_clock::time_point timeout) {
    return WriteParameterizedCommand(command, 0, nullptr, timeout);
}

Status SIM808::SendSimpleParameterizedCommand(
        const char *command, char delimiter, const char *param,
        const char *response, std::chrono::milliseconds timeout) {
    auto end = std::chrono::system_clock::now() + timeout;

    LOG(INFO) << "Sending command: " << command;

    auto status = WriteParameterizedCommand(command, delimiter, param, end);
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

Status SIM808::SendSimpleCommand(const char *command, const char *response,
                                 std::chrono::milliseconds timeout) {
    return SendSimpleParameterizedCommand(command, 0, nullptr, response,
                                          timeout);
}

StatusOr<size_t> SIM808::ReadResponse(
        const char *prefix, char *response, size_t size,
        std::chrono::system_clock::time_point timeout) {
    LOG(INFO) << "Reading response for: " << prefix;

    // Preamble
    auto status = VerifyResponse("\r\n", timeout);
    if (!status.ok())
        return status;

    // Command-specific response start
    status = VerifyResponse("+", timeout);
    if (!status.ok())
        return status;

    status = VerifyResponse(prefix, timeout);
    if (!status.ok())
        return status;

    status = VerifyResponse(": ", timeout);
    if (!status.ok())
        return status;

    // Response content
    size_t total = 0;
    while (1) {
        if (std::chrono::system_clock::now() > timeout)
            return Status(::util::error::Code::DEADLINE_EXCEEDED, "timeout");

        auto statusor = io_->Read();
        if (!statusor.ok()) {
            // Would block. retry.
            if (statusor.status().error_code() ==
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

    return total;
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

    // Command-specific response comes first.
    auto statusor = ReadResponse(prefix, response, size, end);
    if (!statusor.ok())
        return statusor;

    auto total = statusor.Value();

    // Everything is A-OK?
    status = VerifyResponse("\n\r\nOK\r\n", end);
    if (!status.ok())
        return status;

    return total;
}

StatusOr<size_t> SIM808::SendAsynchronousCommand(
        const char *command, const char *prefix, char *response,
        size_t size, std::chrono::milliseconds timeout) {
    auto end = std::chrono::system_clock::now() + timeout;

    auto status = WriteCommand(command, end);
    if (!status.ok()) {
        TryAbort();
        return status;
    }

    // Everything is A-OK?
    status = VerifyResponse("\r\nOK\r\n", end);
    if (!status.ok())
        return status;

    // Command-specific response comes second.
    auto statusor = ReadResponse(prefix, response, size, end);
    if (!statusor.ok())
        return statusor;

    auto total = statusor.Value();

    // Final newline.
    status = VerifyResponse("\n", end);
    if (!status.ok())
        return status;

    return total;
}

}  // namespace sim808
