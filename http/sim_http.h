#ifndef HTTP_SIM_HTTP_H_
#define HTTP_SIM_HTTP_H_

#include <stdint.h>
#include "external/nanopb/util/task/status.h"
#include "external/nanopb/util/task/statusor.h"
#include "http/http.h"
#include "sim808/sim808.h"

namespace http {

class SIMHttp : public Http {
 public:
    SIMHttp(sim808::SIM808 *sim) : sim_(sim) {}

    ::util::StatusOr<HTTPResponse> Get(const char *uri, uint8_t *response_body,
                                       size_t response_size) {
        auto getstatusor = sim_->HTTPGet(uri);
        if (!getstatusor.ok())
            return getstatusor.status();

        auto sim_resp = getstatusor.Value();

        HTTPResponse resp;
        resp.status_code = sim_resp.code;
        resp.body_length = sim_resp.bytes;

        return ReadResponse(resp, response_body, response_size);
    }

    ::util::StatusOr<HTTPResponse> Post(const char *uri, const uint8_t *data,
            size_t data_size, uint8_t *response_body, size_t response_size) {
        auto poststatusor = sim_->HTTPPost(uri, data, data_size);
        if (!poststatusor.ok())
            return poststatusor.status();

        auto sim_resp = poststatusor.Value();

        HTTPResponse resp;
        resp.status_code = sim_resp.code;
        resp.body_length = sim_resp.bytes;

        return ReadResponse(resp, response_body, response_size);
    }

 private:
    // Reads the HTTP response and fills out the rest of resp.
    ::util::StatusOr<HTTPResponse> ReadResponse(HTTPResponse resp,
                                                uint8_t *response_body,
                                                size_t response_size) {
        if (response_body) {
            auto readstatusor = sim_->HTTPRead(response_body, response_size);
            if (!readstatusor.ok())
                return readstatusor.status();

            resp.copied_length = readstatusor.Value();
        } else {
            resp.copied_length = 0;
        }

        return resp;
    }

    sim808::SIM808* sim_;
};

}  // namespace http

#endif  // HTTP_SIM_HTTP_H_
