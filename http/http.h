#ifndef HTTP_HTTP_H_
#define HTTP_HTTP_H_

#include <stdint.h>
#include "external/nanopb/util/task/status.h"
#include "external/nanopb/util/task/statusor.h"

namespace http {

struct HTTPResponse {
    // HTTP response status code.
    uint16_t status_code;

    // Full size of response body.
    size_t body_length;

    // Length of response body copied out.
    size_t copied_length;
};

class Http {
 public:
    virtual ~Http() = default;

    virtual ::util::StatusOr<HTTPResponse> Get(const char *uri, uint8_t *body,
                                               size_t size) = 0;

    virtual ::util::StatusOr<HTTPResponse> Post(const char *uri,
            const uint8_t *data, size_t data_size, uint8_t *response_body,
            size_t response_size) = 0;
};

}  // namespace http

#endif  // HTTP_HTTP_H_
