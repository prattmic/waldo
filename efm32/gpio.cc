#include "efm32/gpio.h"

#include <stdint.h>
#include "efm32/clock.h"
#include "efm32/peripherals.h"
#include "external/com_github_prattmic_nanopb/util/task/status.h"
#include "external/com_github_prattmic_nanopb/util/task/statusor.h"
#include "third_party/gecko_sdk/Device/SiliconLabs/EFM32HG/Include/em_device.h"

namespace efm32 {

constexpr int kPinMax = 16;

// Shift GPIO mode for pin in MODEL/MODEH.
constexpr uint32_t ModeRegValue(uint8_t pin, GPIOMode mode) {
    return mode << (4 * (pin % 8));
}

util::Status GPIOInit() {
    return ClockEnable(PERIPHERAL_GPIO);
}

util::Status GPIOSetMode(GPIOPort port, uint8_t pin, GPIOMode mode) {
    if (port >= GPIOPort::PortMax)
        return util::Status(util::error::INVALID_ARGUMENT, "Invalid port");

    if (pin >= kPinMax)
        return util::Status(util::error::INVALID_ARGUMENT, "Invalid pin");

    GPIO_TypeDef *gpio = reinterpret_cast<GPIO_TypeDef*>(GPIO_BASE);
    if (pin < 8)
        gpio->P[port].MODEL |= ModeRegValue(pin, mode);
    else
        gpio->P[port].MODEH |= ModeRegValue(pin, mode);

    return util::Status::OK;
}

util::Status GPIOOutput(GPIOPort port, uint8_t pin, bool value) {
    if (port >= GPIOPort::PortMax)
        return util::Status(util::error::INVALID_ARGUMENT, "Invalid port");

    if (pin >= kPinMax)
        return util::Status(util::error::INVALID_ARGUMENT, "Invalid pin");

    GPIO_TypeDef *gpio = reinterpret_cast<GPIO_TypeDef*>(GPIO_BASE);

    if (value)
        gpio->P[port].DOUTSET = 1 << pin;
    else
        gpio->P[port].DOUTCLR = 1 << pin;

    return util::Status::OK;
}

util::StatusOr<bool> GPIOValue(GPIOPort port, uint8_t pin) {
    if (port >= GPIOPort::PortMax)
        return util::Status(util::error::INVALID_ARGUMENT, "Invalid port");

    if (pin >= kPinMax)
        return util::Status(util::error::INVALID_ARGUMENT, "Invalid pin");

    GPIO_TypeDef *gpio = reinterpret_cast<GPIO_TypeDef*>(GPIO_BASE);

    return !!(gpio->P[port].DIN & (1 << pin));
}

}  // namespace efm32
