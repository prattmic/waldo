#ifndef EFM32_RTC_H_
#define EFM32_RTC_H_

#include <sys/time.h>
#include "efm32/peripherals.h"
#include "external/com_github_prattmic_nanopb/util/task/status.h"

namespace efm32 {

// Setup MCU RTC.
util::Status RTCInit();

// Return time since power up.
struct timeval RTCTime();

}  // namespace efm32

#endif  // EFM32_RTC_H_
