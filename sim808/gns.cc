#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <chrono>
#include "external/nanopb/util/task/status.h"
#include "external/nanopb/util/task/statusor.h"
#include "sim808/sim808.h"
#include "sim808/gns.h"

namespace sim808 {

StatusOr<bool> SIM808::GNSEnabled() {
    char response[2] = { '\0' };
    auto statusor = SendSynchronousCommand("AT+CGNSPWR?", "CGNSPWR",
                                           response, 2,
                                           std::chrono::milliseconds(100));
    if (!statusor.ok())
        return statusor.status();

    if (statusor.Value() != 1)
        return Status(::util::error::Code::UNKNOWN, "wrong size response");

    if (response[0] == '0')
        return false;

    return true;
}

Status SIM808::GNSEnable(bool enable) {
    if (enable)
        return SendSimpleCommand("AT+CGNSPWR=1", "OK",
                                 std::chrono::milliseconds(100));
    else
        return SendSimpleCommand("AT+CGNSPWR=0", "OK",
                                 std::chrono::milliseconds(100));
}

Status SIM808::GNSInfo(struct GNSInfo *info) {
    char buf[kGNSINFMax+1];
    memset(buf, 0, kGNSINFMax+1);

    auto statusor = SendSynchronousCommand("AT+CGNSINF", "CGNSINF",
                                           buf, kGNSINFMax,
                                           std::chrono::milliseconds(100));
    if (!statusor.ok())
        return statusor.status();

    char *str = buf;

    // GPS run status
    char *field = strsep(&str, ",");
    if (*field != '1')
        return Status(::util::error::Code::UNAVAILABLE,
                      "GNS module not yet running.");

    // Fix status
    field = strsep(&str, ",");
    if (*field) {
        if (*field == '0')
            info->fix = false;
        else
            info->fix = true;
    }

    // UTC date and time
    field = strsep(&str, ",");
    if (*field) {
        if (strlen(field) != kDateSize)
            return Status(::util::error::Code::OUT_OF_RANGE,
                          "invalid UTC date and time size");

        char year[5] = { '\0' };
        memcpy(year, field, 4);
        info->year = strtoul(year, nullptr, 10);

        char month[3] = { '\0' };
        memcpy(month, &field[4], 2);
        info->month = strtoul(month, nullptr, 10);

        char day[3] = { '\0' };
        memcpy(day, &field[6], 2);
        info->day = strtoul(day, nullptr, 10);

        char hour[3] = { '\0' };
        memcpy(hour, &field[8], 2);
        info->hour = strtoul(hour, nullptr, 10);

        char minute[3] = { '\0' };
        memcpy(minute, &field[10], 2);
        info->minute = strtoul(minute, nullptr, 10);

        char second[3] = { '\0' };
        memcpy(second, &field[12], 2);
        info->second = strtoul(second, nullptr, 10);

        // We discard sub-second precision (it is 0 in practice anyways).
    }

    // Latitude
    field = strsep(&str, ",");
    if (*field) {
        if (strlen(field) > kLatitudeMax)
            return Status(::util::error::Code::OUT_OF_RANGE,
                          "invalid latitude size");

        strcpy(info->latitude, field);
    }

    // Longitude
    field = strsep(&str, ",");
    if (*field) {
        if (strlen(field) > kLongitudeMax)
            return Status(::util::error::Code::OUT_OF_RANGE,
                          "invalid longitude size");

        strcpy(info->longitude, field);
    }

    // MSL Altitude
    field = strsep(&str, ",");
    if (*field) {
        if (strlen(field) > kAltitudeMax)
            return Status(::util::error::Code::OUT_OF_RANGE,
                          "invalid altitude size");

        strcpy(info->altitude, field);
    }

    // Ground speed
    field = strsep(&str, ",");
    if (*field) {
        if (strlen(field) > kGroundSpeedMax)
            return Status(::util::error::Code::OUT_OF_RANGE,
                          "invalid ground speed size");

        strcpy(info->ground_speed, field);
    }

    // Heading
    field = strsep(&str, ",");
    if (*field) {
        if (strlen(field) > kHeadingMax)
            return Status(::util::error::Code::OUT_OF_RANGE,
                          "invalid heading size");

        strcpy(info->heading, field);
    }

    // Fix mode (ignored)
    field = strsep(&str, ",");

    // Reserved1 (ignored)
    field = strsep(&str, ",");

    // HDOP (ignored)
    field = strsep(&str, ",");

    // PDOP (ignored)
    field = strsep(&str, ",");

    // VDOP (ignored)
    field = strsep(&str, ",");

    // Reserved2 (ignored)
    field = strsep(&str, ",");

    // GPS satellites in view
    field = strsep(&str, ",");
    if (*field) {
        info->gps_sats_in_view = strtoul(field, nullptr, 10);
    }

    // GNSS satelittes in use
    field = strsep(&str, ",");
    if (*field) {
        info->sats_in_use = strtoul(field, nullptr, 10);
    }

    // GLONASS satellites in view
    field = strsep(&str, ",");
    if (*field) {
        info->glonass_sats_in_view = strtoul(field, nullptr, 10);
    }

    // Remaining fields Reserved3, C/N0 Max, HPA, and VPA all ignored.

    return Status::OK;
}

}  // namespace sim808
