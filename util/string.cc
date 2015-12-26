#include <stdint.h>
#include "util/string.h"

namespace util {

char *uitoa(uint32_t number, char *buf, uint32_t len, uint32_t base) {
    char lookup[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXY";
    int i = 0;

    if (!len) {
        return nullptr;
    }

    if (base < 2 || base > 36) {
        return nullptr;
    }

    do {
        buf[i++] = lookup[number % base];
    } while (--len && (number /= base) > 0);

    if (!len) {
        return nullptr;
    }

    buf[i] = '\0';

    int end = i - 1;
    for (int j = 0; j < end; j++) {
        char temp = buf[j];
        buf[j] = buf[end-j];
        buf[end-j] = temp;
    }

    return buf;
}

}  // namespace util
