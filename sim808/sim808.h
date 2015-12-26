#ifndef SIM808_SIM808_H_
#define SIM808_SIM808_H_

#include <chrono>
#include <memory>
#include <utility>
#include "external/nanopb/util/task/status.h"
#include "external/nanopb/util/task/statusor.h"
#include "io/byteio.h"

namespace sim808 {

using ::util::Status;

class SIM808 {
 public:
    SIM808() : io_(nullptr) {}

    SIM808(std::unique_ptr<io::ByteIO> io) : io_(std::move(io)) {}

    Status Initialize();

 private:
    // Send a command and verify that the response is correct.
    Status SendCommand(const char *command, const char *response,
                       std::chrono::milliseconds timeout);

    Status VerifyResponse(const char *expected,
                          std::chrono::milliseconds timeout);

    // Try to abort a partially written command. May not be successful.
    void TryAbort();

    Status CheckReady();
    Status InitAutoBaud();
    Status DisableCommandEcho();

    std::unique_ptr<io::ByteIO> io_;
};

}  // namespace sim808

#endif  // SIM808_SIM808_H_
