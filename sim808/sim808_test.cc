#include <fcntl.h>
#include <unistd.h>
#include <memory>
#include <utility>
#include "external/googletest/googletest/include/gtest/gtest.h"
#include "external/nanopb/util/task/status.h"
#include "io/linux_byteio.h"
#include "io/logging_byteio.h"
#include "log/log.h"
#include "sim808/gns.h"
#include "sim808/sim808.h"
#include "util/status_test.h"

using io::ByteIO;
using io::LinuxByteIO;
using io::LoggingByteIO;
using sim808::SIM808;
using testing::IsOK;

class SIM808Test : public ::testing::Test {
 protected:
    virtual void SetUp() {
        auto statusor = LinuxByteIO::OpenFile("/dev/ttyACM0");
        ASSERT_OK(statusor.status());
        auto sim_io = std::unique_ptr<ByteIO>(
            new LinuxByteIO(statusor.ConsumeValue()));

        int log_fd = dup(STDERR_FILENO);
        ASSERT_GE(log_fd, 0);

        auto log_io = std::unique_ptr<ByteIO>(new LinuxByteIO(log_fd));
        logging::SetupLogger(std::move(log_io));

        auto io = std::unique_ptr<ByteIO>(
            new LoggingByteIO(std::move(sim_io)));

        sim_ = SIM808(std::move(io));
    }

    SIM808 sim_;
};

TEST_F(SIM808Test, Connect) {
    auto status = sim_.Initialize();
    EXPECT_OK(status);
}

TEST_F(SIM808Test, GNSEnable) {
    auto status = sim_.Initialize();
    ASSERT_OK(status);

    status = sim_.GNSEnable(false);
    ASSERT_OK(status);

    auto statusor = sim_.GNSEnabled();
    ASSERT_OK(statusor.status());
    EXPECT_FALSE(statusor.Value());

    status = sim_.GNSEnable(true);
    ASSERT_OK(status);

    statusor = sim_.GNSEnabled();
    ASSERT_OK(statusor.status());
    EXPECT_TRUE(statusor.Value());

    status = sim_.GNSEnable(false);
    EXPECT_OK(status);
}

TEST_F(SIM808Test, GNSInfo) {
    auto status = sim_.Initialize();
    ASSERT_OK(status);

    status = sim_.GNSEnable(true);
    ASSERT_OK(status);

    struct sim808::GNSInfo info;
    memset(&info, 0, sizeof(info));

    // Module takes a while to start up.
    do {
        status = sim_.GNSInfo(&info);
    } while (!status.ok() &&
             status.error_code() == ::util::error::Code::UNAVAILABLE);

    ASSERT_OK(status);

    LOG(INFO) << "Fix: "                  << info.fix;
    LOG(INFO) << "Year: "                 << info.year;
    LOG(INFO) << "Month: "                << info.month;
    LOG(INFO) << "Day: "                  << info.day;
    LOG(INFO) << "Hour: "                 << info.hour;
    LOG(INFO) << "Minute: "               << info.minute;
    LOG(INFO) << "Second: "               << info.second;
    LOG(INFO) << "Latitude: "             << info.latitude;
    LOG(INFO) << "Longitude: "            << info.longitude;
    LOG(INFO) << "Altitude: "             << info.altitude;
    LOG(INFO) << "Ground speed: "         << info.ground_speed;
    LOG(INFO) << "Heading: "              << info.heading;
    LOG(INFO) << "GPS sats in view: "     << info.gps_sats_in_view;
    LOG(INFO) << "GLONASS sats in view: " << info.glonass_sats_in_view;
    LOG(INFO) << "Sats in use: "          << info.sats_in_use;

    status = sim_.GNSEnable(false);
    EXPECT_OK(status);
}

TEST_F(SIM808Test, GPRSEnable) {
    auto status = sim_.Initialize();
    ASSERT_OK(status);

    // Try to disable. This may return an error if it is already disabled.
    sim_.GPRSEnable(false);

    auto statusor = sim_.GPRSEnabled();
    ASSERT_OK(statusor.status());
    EXPECT_FALSE(statusor.Value());

    status = sim_.GPRSEnable(true);
    ASSERT_OK(status);

    statusor = sim_.GPRSEnabled();
    ASSERT_OK(statusor.status());
    EXPECT_TRUE(statusor.Value());

    status = sim_.GPRSEnable(false);
    EXPECT_OK(status);
}

TEST_F(SIM808Test, HTTPGet) {
    auto status = sim_.Initialize();
    ASSERT_OK(status);

    // Try to disable. These may return an error already disabled.
    sim_.HTTPEnable(false);
    sim_.GPRSEnable(false);

    status = sim_.GPRSEnable(true);
    ASSERT_OK(status);

    status = sim_.HTTPEnable(true);
    ASSERT_OK(status);

    // Returns "hello\nworld\n".
    auto statusor = sim_.HTTPGet("http://pratt.im/hello.txt");
    ASSERT_OK(statusor.status());
    auto resp_status = statusor.Value();
    EXPECT_EQ(200, resp_status.code);
    EXPECT_EQ(12ull, resp_status.bytes);

    uint8_t buf[13] = { '\0' };

    auto statusor2 = sim_.HTTPRead(buf, 12);
    ASSERT_OK(statusor2.status());
    EXPECT_EQ(12ull, statusor2.Value());
    EXPECT_STREQ("hello\nworld\n", reinterpret_cast<char*>(buf));

    status = sim_.HTTPEnable(false);
    EXPECT_OK(status);

    status = sim_.GPRSEnable(false);
    EXPECT_OK(status);
}

TEST_F(SIM808Test, HTTPPost) {
    auto status = sim_.Initialize();
    ASSERT_OK(status);

    // Try to disable. These may return an error already disabled.
    sim_.HTTPEnable(false);
    sim_.GPRSEnable(false);

    status = sim_.GPRSEnable(true);
    ASSERT_OK(status);

    status = sim_.HTTPEnable(true);
    ASSERT_OK(status);

    uint8_t message[] = "hi";
    size_t size = 2;

    // Response body contains the Content-Length of the request.
    auto statusor = sim_.HTTPPost("http://pratt.im/post", message, size);
    ASSERT_OK(statusor.status());
    auto resp_status = statusor.Value();
    EXPECT_EQ(200, resp_status.code);
    EXPECT_EQ(1ull, resp_status.bytes);

    uint8_t buf[2] = { '\0' };

    auto statusor2 = sim_.HTTPRead(buf, 1);
    ASSERT_OK(statusor2.status());
    EXPECT_EQ(1ull, statusor2.Value());
    EXPECT_STREQ("2", reinterpret_cast<char*>(buf));

    status = sim_.HTTPEnable(false);
    EXPECT_OK(status);

    status = sim_.GPRSEnable(false);
    EXPECT_OK(status);
}
