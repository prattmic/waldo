#ifndef EFM32_SIM808_H_
#define EFM32_SIM808_H_

#include <chrono>
#include <chrono>
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
    util::Status SetPower(bool on);

    // Turn power off then on.
    util::Status PowerCycle();
};

}  // namespace efm32

#endif  // EFM32_SIM808_H_
