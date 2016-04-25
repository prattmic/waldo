#ifndef EFM32_GPIO_H_
#define EFM32_GPIO_H_

#include <stdint.h>
#include "external/com_github_prattmic_nanopb/util/task/status.h"
#include "external/com_github_prattmic_nanopb/util/task/statusor.h"
#include "third_party/gecko_sdk/Device/SiliconLabs/EFM32HG/Include/em_device.h"

namespace efm32 {

enum GPIOPort {
    PortA,
    PortB,
    PortC,
    PortD,
    PortE,
    PortF,
    PortMax,
};

enum GPIOMode {
    Disabled                  = _GPIO_P_MODEL_MODE0_DISABLED,
    Input                     = _GPIO_P_MODEL_MODE0_INPUT,
    InputPull                 = _GPIO_P_MODEL_MODE0_INPUTPULL,
    InputPullFilter           = _GPIO_P_MODEL_MODE0_INPUTPULLFILTER,
    PushPull                  = _GPIO_P_MODEL_MODE0_PUSHPULL,
    PushPullDrive             = _GPIO_P_MODEL_MODE0_PUSHPULLDRIVE,
    WiredOr                   = _GPIO_P_MODEL_MODE0_WIREDOR,
    WiredOrPullDown           = _GPIO_P_MODEL_MODE0_WIREDORPULLDOWN,
    WiredAnd                  = _GPIO_P_MODEL_MODE0_WIREDAND,
    WiredAndFilter            = _GPIO_P_MODEL_MODE0_WIREDANDFILTER,
    WiredAndPullUp            = _GPIO_P_MODEL_MODE0_WIREDANDPULLUP,
    WiredAndPullUpFilter      = _GPIO_P_MODEL_MODE0_WIREDANDPULLUPFILTER,
    WiredAndDrive             = _GPIO_P_MODEL_MODE0_WIREDANDDRIVE,
    WiredAndDriveFilter       = _GPIO_P_MODEL_MODE0_WIREDANDDRIVEFILTER,
    WiredAndDrivePullUp       = _GPIO_P_MODEL_MODE0_WIREDANDDRIVEPULLUP,
    WiredAndDrivePullUpFilter = _GPIO_P_MODEL_MODE0_WIREDANDDRIVEPULLUPFILTER,
};

// Initialize GPIO peripheral.
util::Status GPIOInit();

// Set GPIO pin mode.
util::Status GPIOSetMode(GPIOPort port, uint8_t pin, GPIOMode mode);

// Set GPIO output value.
util::Status GPIOOutput(GPIOPort port, uint8_t pin, bool value);

// Get GPIO input value.
util::StatusOr<bool> GPIOValue(GPIOPort port, uint8_t pin);

}  // namespace efm32

#endif  // EFM32_GPIO_H_
