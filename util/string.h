#ifndef UTIL_STRING_H_
#define UTIL_STRING_H_

#include <stdint.h>

namespace util {

// Convert uint32_t to string.
char *uitoa(uint32_t number, char *buf, uint32_t len, uint32_t base, uint32_t pad);

// Convert int32_t to string.
char *itoa(int32_t number, char *buf, uint32_t len, uint32_t base, uint32_t pad);

}  // namespace util

#endif  // UTIL_STRING_H_
