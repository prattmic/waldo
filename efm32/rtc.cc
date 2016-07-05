#include "efm32/gpio.h"

#include <stdint.h>
#include <sys/time.h>
#include "efm32/clock.h"
#include "efm32/peripherals.h"
#include "external/com_github_prattmic_nanopb/util/task/status.h"
#include "third_party/gecko_sdk/Device/SiliconLabs/EFM32HG/Include/em_device.h"

namespace efm32 {

constexpr int kRTCClockBaseHz = 32768;
constexpr int kRTCClockPrescale = 256;

constexpr int kMicrosecondPerSecond = 1000*1000;

util::Status RTCInit() {
    auto status = ClockEnable(PERIPHERAL_RTC);
    if (!status.ok())
        return status;

    CMU->LFAPRESC0 |= CMU_LFAPRESC0_RTC_DIV256;  // 7.81ms precision
    RTC->CTRL |= RTC_CTRL_EN;
}

// TODO(prattmic): handle overflow
struct timeval RTCTime() {
    uint32_t count = RTC->CNT;

    struct timeval tv;
    tv.tv_sec = (count * kRTCClockPrescale) / kRTCClockBaseHz;

    uint64_t rem = (count * kRTCClockPrescale) % kRTCClockBaseHz;
    tv.tv_usec = (rem * kMicrosecondPerSecond) / kRTCClockBaseHz;

    return tv;
}

}  // namespace efm32
