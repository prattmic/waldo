#ifndef SIM808_HTTP_H_
#define SIM808_HTTP_H_

#include <stddef.h>

namespace sim808 {

struct HTTPResponseStatus {
    // HTTP response code.
    int code;
    // Bytes received.
    size_t bytes;
};

}  // namespace sim808

#endif  // SIM808_HTTP_H_
