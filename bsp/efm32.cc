#include <sys/time.h>
#include "efm32/rtc.h"

extern "C" {

int _gettimeofday(struct timeval *tv, struct timezone *tz) {
    if (tv) {
        *tv = efm32::RTCTime();
    }
    return 0;
}

}  // extern "C"
