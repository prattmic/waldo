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

}  // namespace efm32
