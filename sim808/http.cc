#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "external/nanopb/util/task/status.h"
#include "sim808/http.h"
#include "sim808/sim808.h"
#include "util/string.h"

namespace sim808 {

// Note: we always use GPRS bearer CID 1.

// Max response size for a SAPBR query.
// '1,4,"255.255.255.255"' -> 22 bytes
constexpr size_t kSAPBRQueryMax = 22;

// SAPBR query status 1 indicates that the bearer is connected.
constexpr char kBearerStatusConnected = '1';

// Maximum supported size of request/response body.
// It could be larger, we just don't support that.
constexpr size_t kHTTPDataMax = 99999;

// Maximum response size for an HTTPACTION command:
// 0,604,99999 -> 11
constexpr size_t kHTTPACTIONRespMax = 11;

// Maximum response size for the first line of HTTPREAD, the response
// size:
// 99999 -> 5
constexpr size_t kHTTPDataMaxStrSize = 5;

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

Status SIM808::HTTPEnable(bool enable) {
    if (enable)
        return SendSimpleCommand("AT+HTTPINIT", "OK",
                                 std::chrono::milliseconds(100));
    else
        return SendSimpleCommand("AT+HTTPTERM", "OK",
                                 std::chrono::milliseconds(100));
}

Status SIM808::HTTPRequestSetup(const char *uri) {
    auto status = SendSimpleCommand("AT+HTTPPARA=\"CID\",1", "OK",
                                    std::chrono::milliseconds(100));
    if (!status.ok())
        return status;

    return SendSimpleParameterizedCommand("AT+HTTPPARA=\"URL\",\"%\"",
                                          '%', uri, "OK",
                                          std::chrono::milliseconds(100));
}

StatusOr<HTTPResponseStatus> SIM808::HTTPAction(HTTPMethod method) {
    const char *command;

    switch (method) {
    case GET:
        command = "AT+HTTPACTION=0";
        break;
    case POST:
        command = "AT+HTTPACTION=1";
        break;
    case HEAD:
        command = "AT+HTTPACTION=2";
        break;
    default:
        return Status(::util::error::Code::FAILED_PRECONDITION,
                      "Uknown HTTPAction");
    }

    char response[kHTTPACTIONRespMax+1] = { '\0' };
    auto statusor = SendAsynchronousCommand(command, "HTTPACTION",
                                           response, kHTTPACTIONRespMax,
                                           std::chrono::milliseconds(30000));
    if (!statusor.ok())
        return statusor.status();

    if (statusor.Value() >= kHTTPACTIONRespMax)
        return Status(::util::error::Code::UNKNOWN, "response too large");

    HTTPResponseStatus resp_status;

    char *str = response;

    // HTTP method (ignored)
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

StatusOr<HTTPResponseStatus> SIM808::HTTPGet(const char *uri) {
    auto status = HTTPRequestSetup(uri);
    if (!status.ok())
        return status;

    return HTTPAction(GET);
}

StatusOr<HTTPResponseStatus> SIM808::HTTPPost(const char *uri,
                                              const uint8_t *data,
                                              size_t size) {
    if (size > kHTTPDataMax)
        return Status(::util::error::Code::FAILED_PRECONDITION,
                      "POST data size too large");

    auto status = HTTPRequestSetup(uri);
    if (!status.ok())
        return status;

    // Clear any data that may have been written previously.
    status = SendSimpleCommand("AT+HTTPDATA=0,1000", "OK",
                               std::chrono::milliseconds(100));
    if (!status.ok())
        return status;

    // Prepare to write out the data.
    char size_buf[kHTTPDataMaxStrSize+1] = { '\0' };
    ::util::uitoa(size, size_buf, kHTTPDataMaxStrSize, 10);

    // We have about 100ms from this point.
    auto end = std::chrono::system_clock::now()
        + std::chrono::milliseconds(100);

    status = WriteParameterizedCommand("AT+HTTPDATA=%,100000", '%', size_buf,
                                       end);
    if (!status.ok())
        return status;

    // Wait until the device is ready to read.
    status = VerifyResponse("\r\nDOWNLOAD\r\n", end);
    if (!status.ok())
        return status;

    // Actually write out the data.
    while (size) {
        if (std::chrono::system_clock::now() > end)
            return Status(::util::error::Code::DEADLINE_EXCEEDED, "timeout");

        status = io_->Write(*data);
        if (!status.ok()) {
            // Would block. retry.
            if (status.error_code() == ::util::error::Code::RESOURCE_EXHAUSTED)
                continue;

            return status;
        }

        size--;
        data++;
    }

    // Good to go?
    status = VerifyResponse("\r\nOK\r\n", end);
    if (!status.ok())
        return status;

    return HTTPAction(POST);
}

StatusOr<size_t> SIM808::HTTPRead(uint8_t *response, size_t size) {
    // HTTPREAD has a special response format, so we handle it explicitly,
    // rather than using one of the Send*Command functions.
    //
    // Response is in the form:
    //
    // +HTTPREAD: <bytes to read>
    // multiple
    // lines
    // adding to
    // <bytes to read>
    //
    // OK

    auto end = std::chrono::system_clock::now()
        + std::chrono::milliseconds(100);

    auto status = WriteCommand("AT+HTTPREAD", end);
    if (!status.ok()) {
        TryAbort();
        return status;
    }

    char data_len_buf[kHTTPDataMaxStrSize+1] = { '\0' };
    auto statusor = ReadResponse("HTTPREAD", data_len_buf,
                                 kHTTPDataMaxStrSize, end);
    if (!statusor.ok())
        return statusor.status();

    if (statusor.Value() >= kHTTPDataMaxStrSize)
        return Status(::util::error::Code::UNKNOWN, "response too large");

    size_t data_len = strtoull(data_len_buf, nullptr, 10);

    // Eat the newline.
    status = VerifyResponse("\n", end);
    if (!status.ok())
        return status;

    // Response content
    size_t total = 0;
    while (data_len) {
        if (std::chrono::system_clock::now() > end)
            return Status(::util::error::Code::DEADLINE_EXCEEDED, "timeout");

        auto statusor = io_->Read();
        if (!statusor.ok()) {
            // Would block. retry.
            if (statusor.status().error_code() ==
                ::util::error::Code::RESOURCE_EXHAUSTED)
                continue;

            return statusor.status();
        }

        uint8_t c = statusor.Value();

        // We keep consuming the response even if there is no more room
        // in the buffer, so we can see if the command itself was successful.
        if (size > 0) {
            *response++ = c;
            total++;
            size--;
        }

        data_len--;
    }

    // Everything is A-OK?
    status = VerifyResponse("\r\nOK\r\n", end);
    if (!status.ok())
        return status;

    return total;
}

}  // namespace sim808
