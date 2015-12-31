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

    ::util::StatusOr<HTTPResponse> Get(const char *uri, char *body,
                                       size_t size) {
        auto getstatusor = sim_->HTTPGet(uri);
        if (!getstatusor.ok())
            return getstatusor.status();

        auto sim_resp = getstatusor.Value();

        HTTPResponse ret;
        ret.status_code = sim_resp.code;
        ret.body_length = sim_resp.bytes;

        if (body) {
            auto readstatusor = sim_->HTTPRead(body, size);
            if (!readstatusor.ok())
                return readstatusor.status();

            ret.copied_length = readstatusor.Value();
        } else {
            ret.copied_length = 0;
        }

        return ret;
    }

    ::util::StatusOr<HTTPResponse> Post(const char *uri, const uint8_t *data,
            size_t data_size, char *response_body, size_t response_size) {
        auto poststatusor = sim_->HTTPPost(uri, data, data_size);
        if (!poststatusor.ok())
            return poststatusor.status();

        auto sim_resp = poststatusor.Value();

        HTTPResponse ret;
        ret.status_code = sim_resp.code;
        ret.body_length = sim_resp.bytes;

        if (response_body) {
            auto readstatusor = sim_->HTTPRead(response_body, response_size);
            if (!readstatusor.ok())
                return readstatusor.status();

            ret.copied_length = readstatusor.Value();
        } else {
            ret.copied_length = 0;
        }

        return ret;
    }

 private:
    sim808::SIM808* sim_;
};

}  // namespace http

#endif  // HTTP_SIM_HTTP_H_
