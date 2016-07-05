#ifndef HTTP_CPR_HTTP_H_
#define HTTP_CPR_HTTP_H_

#include <stdint.h>
#include "external/cpr/include/cpr/cpr.h"
#include "external/com_github_prattmic_nanopb/util/task/status.h"
#include "external/com_github_prattmic_nanopb/util/task/statusor.h"
#include "http/http.h"

namespace http {

class CPRHttp : public Http {
 public:
    CPRHttp() {}

    ::util::StatusOr<HTTPResponse> Get(const char *uri, uint8_t *body,
                                       size_t size) {
        auto response = cpr::Get(cpr::Url{uri});

        HTTPResponse ret;
        ret.status_code = response.status_code;
        ret.body_length = response.text.length();

        ret.copied_length = response.text.copy(reinterpret_cast<char*>(body),
                                               size);

        return ret;
    }

    ::util::StatusOr<HTTPResponse> Post(const char *uri, const uint8_t *data,
            size_t data_size, uint8_t *response_body, size_t response_size) {
        auto response = cpr::Post(cpr::Url{uri},
                                  cpr::Body{reinterpret_cast<const char*>(data),
                                            data_size});

        HTTPResponse ret;
        ret.status_code = response.status_code;
        ret.body_length = response.text.length();

        if (response_body)
            ret.copied_length = response.text.copy(
                    reinterpret_cast<char*>(response_body), response_size);
        else
            ret.copied_length = 0;

        return ret;
    }

};

}  // namespace http

#endif  // HTTP_CPR_HTTP_H_
