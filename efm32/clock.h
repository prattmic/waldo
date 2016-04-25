#ifndef EFM32_CLOCK_H_
#define EFM32_CLOCK_H_

#include "efm32/peripherals.h"
#include "external/com_github_prattmic_nanopb/util/task/status.h"

namespace efm32 {

// Setup MCU clocks.
util::Status ClockInit();

// Enable peripheral clock.
util::Status ClockEnable(enum Peripherals peripheral);

}  // namespace efm32

#endif  // EFM32_CLOCK_H_
