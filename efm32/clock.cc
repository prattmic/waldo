#include "efm32/clock.h"

#include "efm32/peripherals.h"
#include "external/com_github_prattmic_nanopb/util/task/status.h"
#include "third_party/gecko_sdk/Device/SiliconLabs/EFM32HG/Include/em_device.h"

namespace efm32 {

util::Status ClockInit() {
    static bool ready = false;
    if (ready)
        return util::Status::OK;

    // Wait for clock to be ready
    while (!(CMU->STATUS & CMU_STATUS_HFRCORDY));

    ready = true;
    return util::Status::OK;
}

util::Status ClockEnable(enum Peripherals peripheral) {
    switch (peripheral) {
    case PERIPHERAL_GPIO:
        CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_GPIO;
        break;
    case PERIPHERAL_USART0:
        CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_USART0;
        break;
    case PERIPHERAL_USART1:
        CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_USART1;
        break;
    default:
        return util::Status(util::error::INVALID_ARGUMENT,
                            "Unknown peripheral");
    }

    return util::Status::OK;
}

}  // namespace efm32
