#include "efm32/sim808.h"

#include <memory>
#include "efm32/gpio.h"
#include "io/byteio.h"
#include "log/log.h"
#include "sim808/sim808.h"

namespace efm32 {

EFMSIM808::EFMSIM808(std::unique_ptr<io::ByteIO> io)
    : sim808::SIM808(std::move(io)) {
    auto status = GPIOInit();
    if (!status.ok()) {
        LOG(ERROR) << "GPIOInit failed: " << status.error_message();
        return;
    }

    // PWRSTAT: PC1 to pull-down input.
    status = GPIOOutput(GPIOPort::PortC, 1, false);
    if (!status.ok()) {
        LOG(ERROR) << "SIM808 PWRSTAT GPIOOutput failed: "
                   << status.error_message();
        return;
    }

    status = GPIOSetMode(GPIOPort::PortC, 1, GPIOMode::InputPull);
    if (!status.ok()) {
        LOG(ERROR) << "SIM808 PWRSTAT GPIOSetMode failed: "
                   << status.error_message();
        return;
    }

    // PWRKEY: PC0 to push-pull output.
    status = GPIOOutput(GPIOPort::PortC, 0, false);
    if (!status.ok()) {
        LOG(ERROR) << "SIM808 PWRKEY GPIOOutput failed: "
                   << status.error_message();
        return;
    }

    status = GPIOSetMode(GPIOPort::PortC, 1, GPIOMode::PushPull);
    if (!status.ok()) {
        LOG(ERROR) << "SIM808 PWRKEY GPIOSetMode failed: "
                   << status.error_message();
        return;
    }
}

util::Status EFMSIM808::SetPower(bool on, std::chrono::system_clock::time_point timeout) {
    LOG(INFO) << "Powering: " << on;

    auto statusor = PowerStatus();
    if (!statusor.ok())
        return statusor.status();

    // Already in desired state?
    if (statusor.Value() == on)
        return util::Status::OK;

    // Hold PWRKEY high until PWRSTAT goes high.
    auto status = GPIOOutput(GPIOPort::PortC, 0, true);
    if (!status.ok())
        return status;

    while (statusor.ok() && statusor.Value() != on) {
        if (std::chrono::system_clock::now() > timeout) {
            GPIOOutput(GPIOPort::PortC, 0, false);
            return util::Status(util::error::Code::DEADLINE_EXCEEDED,
                                "SetPower timeout");
        }

        statusor = PowerStatus();
    }

    // PWRKEY back to low.
    status = GPIOOutput(GPIOPort::PortC, 0, false);
    if (!status.ok())
        return status;

    // Power is now either in the desired state or there was an error.
    return statusor.status();
}

util::Status EFMSIM808::SetPowerRetry(bool on) {
    // The manual says this can take up to 33s.
    int timeouts[] = {1, 10, 33};

    util::Status status;
    for (int i = 0; i < sizeof(timeouts)/sizeof(timeouts[0]); i++) {
        auto end = std::chrono::system_clock::now()
            + std::chrono::seconds(timeouts[i]);
        status = SetPower(on, end);
        if (status.error_code() != util::error::Code::DEADLINE_EXCEEDED)
            return status;
    }

    return status;
}

util::Status EFMSIM808::PowerCycle() {
    auto statusor = PowerStatus();
    if (!statusor.ok())
        return statusor.status();

    // Power off if not already off.
    if (statusor.Value()) {
        auto status = SetPowerRetry(false);
        if (!status.ok())
            return status;
    }

    auto now = std::chrono::system_clock::now();
    auto end = now + std::chrono::seconds(1);
    while (std::chrono::system_clock::now() < end);

    return SetPowerRetry(true);
}

}  // namespace efm32
