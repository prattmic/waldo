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
    for (int j = 0; j <= end/2; j++) {
        char temp = buf[j];
        buf[j] = buf[end-j];
        buf[end-j] = temp;
    }

    return buf;
}

// Safely takes absolute value of an int, properly handling INT_MIN.
static uint32_t int_abs(int32_t n) {
    if (n > 0) {
        return n;
    }
    else {
        return UINT32_MAX - (uint32_t) n + 1;
    }
}

char *itoa(int32_t number, char *buf, uint32_t len, uint32_t base) {
    int32_t i = 0;

    if (!len) {
        return nullptr;
    }

    if (base < 2 || base > 36) {
        return nullptr;
    }

    if (number < 0) {
        buf[i++] = '-';
        len--;
    }

    number = int_abs(number);

    return uitoa(number, &buf[i], len, base);
}

}  // namespace util
