#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <memory>
#include <utility>
#include "external/googletest/googletest/include/gtest/gtest.h"
#include "external/nanopb/util/task/status.h"
#include "io/linux_byteio.h"
#include "io/logging_byteio.h"
#include "sim808/gns.h"
#include "sim808/sim808.h"

using io::ByteIO;
using io::LinuxByteIO;
using io::LoggingByteIO;
using sim808::SIM808;

class SIM808Test : public ::testing::Test {
 protected:
    virtual void SetUp() {
        auto statusor = LinuxByteIO::OpenFile("/dev/ttyACM0");
        ASSERT_TRUE(statusor.ok()) << statusor.status().ToString();
        auto sim_io = std::unique_ptr<ByteIO>(
            new LinuxByteIO(statusor.ConsumeValue()));

        int log_fd = dup(STDERR_FILENO);
        ASSERT_GE(log_fd, 0);

        auto log_io = std::unique_ptr<ByteIO>(new LinuxByteIO(log_fd));

        auto io = std::unique_ptr<ByteIO>(
            new LoggingByteIO(std::move(sim_io), std::move(log_io)));

        sim_ = SIM808(std::move(io));
    }

    SIM808 sim_;
};

TEST_F(SIM808Test, Connect) {
    auto status = sim_.Initialize();
    EXPECT_TRUE(status.ok()) << status.error_message();
}

TEST_F(SIM808Test, GNSEnable) {
    auto status = sim_.Initialize();
    ASSERT_TRUE(status.ok()) << status.error_message();

    status = sim_.GNSEnable(false);
    ASSERT_TRUE(status.ok()) << status.error_message();

    auto statusor = sim_.GNSEnabled();
    ASSERT_TRUE(statusor.ok()) << statusor.status().error_message();
    EXPECT_FALSE(statusor.Value());

    status = sim_.GNSEnable(true);
    ASSERT_TRUE(status.ok()) << status.error_message();

    statusor = sim_.GNSEnabled();
    ASSERT_TRUE(statusor.ok()) << statusor.status().error_message();
    EXPECT_TRUE(statusor.Value());

    status = sim_.GNSEnable(false);
    EXPECT_TRUE(status.ok()) << status.error_message();
}

TEST_F(SIM808Test, GNSInfo) {
    auto status = sim_.Initialize();
    ASSERT_TRUE(status.ok()) << status.error_message();

    status = sim_.GNSEnable(true);
    ASSERT_TRUE(status.ok()) << status.error_message();

    struct sim808::GNSInfo info;
    memset(&info, 0, sizeof(info));

    // Module takes a while to start up.
    do {
        status = sim_.GNSInfo(&info);
    } while (!status.ok() &&
             status.error_code() == ::util::error::Code::UNAVAILABLE);

    ASSERT_TRUE(status.ok()) << status.error_message();

    std::cerr << "Fix: "                  << info.fix << "\n";
    std::cerr << "Year: "                 << info.year << "\n";
    std::cerr << "Month: "                << info.month << "\n";
    std::cerr << "Day: "                  << info.day << "\n";
    std::cerr << "Hour: "                 << info.hour << "\n";
    std::cerr << "Minute: "               << info.minute << "\n";
    std::cerr << "Second: "               << info.second << "\n";
    std::cerr << "Latitude: "             << info.latitude << "\n";
    std::cerr << "Longitude: "            << info.longitude << "\n";
    std::cerr << "Altitude: "             << info.altitude << "\n";
    std::cerr << "Ground speed: "         << info.ground_speed << "\n";
    std::cerr << "Heading: "              << info.heading << "\n";
    std::cerr << "GPS sats in view: "     << info.gps_sats_in_view << "\n";
    std::cerr << "GLONASS sats in view: " << info.glonass_sats_in_view << "\n";
    std::cerr << "Sats in use: "          << info.sats_in_use << "\n";
}
