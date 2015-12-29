#ifndef SIM808_SIM808_H_
#define SIM808_SIM808_H_

#include <chrono>
#include <memory>
#include <utility>
#include "external/nanopb/util/task/status.h"
#include "external/nanopb/util/task/statusor.h"
#include "io/byteio.h"
#include "sim808/gns.h"
#include "sim808/http.h"

namespace sim808 {

using ::util::Status;
using ::util::StatusOr;

class SIM808 {
 public:
    SIM808() : io_(nullptr) {}

    SIM808(std::unique_ptr<io::ByteIO> io) : io_(std::move(io)) {}

    Status Initialize();

    // Check if the GNS module is enabled.
    StatusOr<bool> GNSEnabled();

    // Enable/disable GNS module.
    Status GNSEnable(bool enable);

    // Read current GNS info. Return UNAVAILABLE if the module is not yet
    // ready.
    Status GNSInfo(struct GNSInfo *info);

    // Check if GPRS is enabled and connected.
    StatusOr<bool> GPRSEnabled();

    // Enable/disable GPRS connection. May return an error if already
    // enabled/disabled. Must be enabled before making HTTP requests.
    Status GPRSEnable(bool enable);

    // Enable/disable HTTP operation. Must be enabled before making HTTP
    // requests. May return an error if already enabled/disabled.
    Status HTTPEnable(bool enable);

    // Make an HTTP GET request. If successful, the response body can
    // be read with HTTPRead.
    StatusOr<HTTPResponseStatus> HTTPGet(const char *uri);

 private:
    // Send a command and with a fixed expected response.
    Status SendSimpleCommand(const char *command, const char *response,
                             std::chrono::milliseconds timeout);

    // Same as SendSimpleCommand, but with WriteParameterizedCommand
    // semantics.
    Status SendSimpleParameterizedCommand(
            const char *command, char delimiter, const char *param,
            const char *response, std::chrono::milliseconds timeout);

    // Read a command-specific response in the form:
    // +prefix: <this goes in response>
    //
    // The reponse after +prefix: must be on a single line.
    //
    // Returns the number of bytes written to response. Bytes that do not fit
    // in response are dropped.
    StatusOr<size_t> ReadResponse(const char *prefix, char *response,
            size_t size, std::chrono::system_clock::time_point timeout);

    // Send a command and return the synchronous command-specific response
    // in response.
    //
    // This applies to commands that responses in the form:
    // +prefix: <this goes in response>
    //
    // OK
    //
    // The reponse after +prefix: must be on a single line.
    //
    // Returns the number of bytes written to response. Bytes that do not fit
    // in response are dropped. Returns an error if command does not end in
    // OK.
    StatusOr<size_t> SendSynchronousCommand(const char *command,
                                            const char *prefix,
                                            char response[],
                                            size_t size,
                                            std::chrono::milliseconds timeout);

    // Send a command and return the synchronous command-specific response
    // in response.
    //
    // This applies to commands that responses in the form:
    // OK
    //
    // +prefix: <this goes in response>
    //
    // The reponse after +prefix: must be on a single line.
    //
    // Returns the number of bytes written to response. Bytes that do not fit
    // in response are dropped. Returns an error if command does not end in
    // OK.
    StatusOr<size_t> SendAsynchronousCommand(const char *command,
                                             const char *prefix,
                                             char response[],
                                             size_t size,
                                             std::chrono::milliseconds timeout);

    // Write command, adding trailing \r.
    Status WriteCommand(const char *command,
                        std::chrono::system_clock::time_point timeout);

    // Write command, adding trailing \r, with an optional parameter. If
    // delimiter and param are set, all instances of delimiter is replaced with
    // param.
    Status WriteParameterizedCommand(
            const char *command, char delimiter, const char *param,
            std::chrono::system_clock::time_point timeout);

    // Try to consume the rest of the line (up to '\n') to keep future
    // readers from choking on it. No promises, though.
    void TryConsumeLine(std::chrono::system_clock::time_point timeout);

    Status VerifyResponse(const char *expected,
                          std::chrono::system_clock::time_point timeout);

    // Try to abort a partially written command. May not be successful.
    void TryAbort();

    Status CheckReady();
    Status InitAutoBaud();
    Status DisableCommandEcho();

    std::unique_ptr<io::ByteIO> io_;
};

}  // namespace sim808

#endif  // SIM808_SIM808_H_
