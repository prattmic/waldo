#ifndef EFM32_SIM808_H_
#define EFM32_SIM808_H_

#include <memory>
#include "efm32/gpio.h"
#include "external/com_github_prattmic_nanopb/util/task/status.h"
#include "external/com_github_prattmic_nanopb/util/task/statusor.h"
#include "io/byteio.h"
#include "sim808/sim808.h"

namespace efm32 {

class EFMSIM808 : public sim808::SIM808 {
 public:
    EFMSIM808(std::unique_ptr<io::ByteIO> io);

    // Returns true if SIM808 is powered.
    util::StatusOr<bool> PowerStatus() {
        return GPIOValue(GPIOPort::PortC, 1);
    }

    // Turn power on or off.
    // TODO(prattmic): timeout
    util::Status SetPower(bool on) {
        auto statusor = PowerStatus();
        if (!statusor.ok())
            return statusor.status();

        // Already in desired state?
        if (statusor.Value() == on)
            return util::Status::OK;

        // Hold PWRKEY high until PWRSTAT goes high.
        auto status = GPIOOutput(GPIOPort::PortC, 1, true);
        if (!status.ok())
            return status;

        while (statusor.ok() && statusor.Value() != on) {
            statusor = PowerStatus();
        }

        // PWRKEY back to low.
        status = GPIOOutput(GPIOPort::PortC, 1, false);
        if (!status.ok())
            return status;

        // Power is now either in the desired state or there was an error.
        return statusor.status();
    }

    util::Status PowerCycle() {
        auto statusor = PowerStatus();
        if (!statusor.ok())
            return statusor.status();

        // Power off if not already off.
        if (statusor.Value()) {
            auto status = SetPower(false);
            if (!status.ok())
                return status;
        }

        return SetPower(true);
    }
};

}  // namespace efm32

#endif  // EFM32_SIM808_H_
