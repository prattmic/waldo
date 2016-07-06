#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <chrono>
#include "external/com_github_prattmic_nanopb/util/task/status.h"
#include "external/com_github_prattmic_nanopb/util/task/statusor.h"
#include "log/log.h"
#include "sim808/sim808.h"

namespace sim808 {

// 4 bytes for required field, plus extra for operator name.
constexpr int kCOPSMax = 50;

StatusOr<bool> SIM808::NetworkConnected() {
    char buf[kCOPSMax+1];
    memset(buf, 0, kCOPSMax+1);

    auto statusor = SendSynchronousCommand("AT+CREG?", "CREG",
                                           buf, kCOPSMax,
                                           std::chrono::milliseconds(1000));
    if (!statusor.ok())
        return statusor.status();

    char *str = buf;

    // Registration result code
    char *field = strsep(&str, ",");
    LOG(INFO) << "n: " << field;

    // Registration status
    field = strsep(&str, ",");
    // If format is omitted, we aren't connected.
    switch (*field) {
    case '\0':
        LOG(WARNING) << "stat field missing when expected";
        return false;
    case '0':
        LOG(INFO) << "Not connected, not searching for network";
        return false;
    case '1':
        LOG(INFO) << "Connected to home network";
        return true;
    case '2':
        LOG(INFO) << "Not connected, searching for network";
        return false;
    case '3':
        LOG(INFO) << "Connection denied";
        return false;
    case '4':
        LOG(INFO) << "Connection unknown";
        return false;
    case '5':
        LOG(INFO) << "Connected, roaming";
        return true;
    default:
        LOG(INFO) << "Unknown status: " << field;
        return false;
    }
}

}  // namespace sim808
