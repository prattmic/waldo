#include <stdint.h>
#include <string.h>
#include "external/nanopb/util/task/status.h"
#include "sim808/http.h"
#include "sim808/sim808.h"

namespace sim808 {

// Note: we always use GPRS bearer CID 1.

// Max response size for a SAPBR query.
// '1,4,"255.255.255.255"' -> 22 bytes
constexpr size_t kSAPBRQueryMax = 22;

// SAPBR query status 1 indicates that the bearer is connected.
constexpr char kBearerStatusConnected = '1';

// Maximum response size for an HTTPACTION command:
// 0,604,99999 -> 11
// The response could be bigger than the above if more data
// is received, but we do not support that.
constexpr size_t kHTTPACTIONRespMax = 11;

StatusOr<bool> SIM808::GPRSEnabled() {
    char response[kSAPBRQueryMax+1] = { '\0' };
    auto statusor = SendSynchronousCommand("AT+SAPBR=2,1", "SAPBR",
                                           response, kSAPBRQueryMax,
                                           std::chrono::milliseconds(100));
    if (!statusor.ok())
        return statusor.status();

    // The status is in byte 2, so we must have read at least that many bytes.
    if (statusor.Value() < 3)
        return Status(::util::error::Code::UNKNOWN, "response too small");

    if (response[2] == kBearerStatusConnected)
        return true;
    else
        return false;
}

Status SIM808::GPRSEnable(bool enable) {
    if (enable) {
        // Set to GPRS mode.
        auto status = SendSimpleCommand("AT+SAPBR=3,1,\"Contype\", \"GPRS\"",
                                        "OK", std::chrono::milliseconds(100));
        if (!status.ok())
            return status;

        // Open a GPRS bearer. This may take a while.
        return SendSimpleCommand("AT+SAPBR=1,1", "OK",
                                 std::chrono::milliseconds(5000));
    } else {
        // Close the GPRS bearer. This may take a while.
        return SendSimpleCommand("AT+SAPBR=0,1", "OK",
                                 std::chrono::milliseconds(5000));
    }
}

StatusOr<HTTPResponseStatus> SIM808::HTTPGet(const char *uri) {
    auto status = SendSimpleCommand("AT+HTTPINIT", "OK",
                                    std::chrono::milliseconds(100));
    // FIXME(prattmic): we need to HTTPTERM when done.
    //if (!status.ok())
    //    return status;

    status = SendSimpleCommand("AT+HTTPPARA=\"CID\",1", "OK",
                               std::chrono::milliseconds(100));
    if (!status.ok())
        return status;

    status = SendSimpleParameterizedCommand(
            "AT+HTTPPARA=\"URL\",\"%\"", '%', uri, "OK",
            std::chrono::milliseconds(100));
    if (!status.ok())
        return status;

    char response[kHTTPACTIONRespMax+1] = { '\0' };
    auto statusor = SendAsynchronousCommand("AT+HTTPACTION=0", "HTTPACTION",
                                           response, kHTTPACTIONRespMax,
                                           std::chrono::milliseconds(30000));
    if (!statusor.ok())
        return statusor.status();

    if (statusor.Value() >= kHTTPACTIONRespMax)
        return Status(::util::error::Code::UNKNOWN, "response too large");

    HTTPResponseStatus resp_status;

    char *str = response;

    // HTTP method (ignored, it's GET)
    char *field = strsep(&str, ",");

    // Status code
    field = strsep(&str, ",");
    if (*field)
        resp_status.code = strtoul(field, nullptr, 10);

    // Data length
    field = strsep(&str, ",");
    if (*field)
        resp_status.bytes = strtoul(field, nullptr, 10);

    return resp_status;
}

}  // namespace sim808
