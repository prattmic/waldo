#ifndef SIM808_GNS_H_
#define SIM808_GNS_H_

namespace sim808 {

// Maximum buffer sizes needed to hold GNS info fields.
// From SIM800 Series GNSS Application Note, Table 2-3:
// "AT+CGNSINF return Parameters"
// https://www.adafruit.com/datasheets/SIM800%20Series_GNSS_Application%20Note%20V1.00.pdf
constexpr int kLatitudeMax = 10;
constexpr int kLongitudeMax = 11;
constexpr int kAltitudeMax = 8;
constexpr int kGroundSpeedMax = 6;
constexpr int kHeadingMax = 6;

// UTC date and time field should be exactly this size (not including NULL
// byte).
constexpr int kDateSize = 18;

// Maximum buffer size needed for CGNSINF return value.
constexpr int kGNSINFMax = 94;

struct GNSInfo {
    // GNS Fix
    bool fix;

    // UTC date/time
    uint16_t year;
    uint16_t month;
    uint16_t day;
    uint16_t hour;
    uint16_t minute;
    uint16_t second;

    // Latitude/longitude in degrees, with 6 decimal points of precision.
    // Since we don't expect to need to process these beyond sending them
    // to the internet, we don't incur the expense of converting the floating
    // point values.
    char latitude[kLatitudeMax+1];
    char longitude[kLongitudeMax+1];

    // MSL altitude in meters. Left as a string, as above.
    char altitude[kAltitudeMax+1];

    // Ground speed in km/hour. Left as a string, as above.
    char ground_speed[kGroundSpeedMax+1];

    // Course heading in degrees. Left as a string, as above.
    char heading[kHeadingMax+1];

    uint16_t gps_sats_in_view;
    uint16_t glonass_sats_in_view;
    uint16_t sats_in_use;
};

}  // namespace sim808

#endif  // SIM808_GNS_H_
