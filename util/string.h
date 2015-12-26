#ifndef UTIL_STRING_H_
#define UTIL_STRING_H_

#include <stdint.h>

namespace util {

// Convert int to string.
char *uitoa(uint32_t number, char *buf, uint32_t len, uint32_t base);

}  // namespace util

#endif  // UTIL_STRING_H_
