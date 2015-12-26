#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <memory>
#include <utility>
#include "external/googletest/googletest/include/gtest/gtest.h"
#include "external/nanopb/util/task/status.h"
#include "io/linux_byteio.h"
#include "io/logging_byteio.h"
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

TEST_F(SIM808Test, GNSEnabled) {
    auto status = sim_.Initialize();
    ASSERT_TRUE(status.ok()) << status.error_message();

    auto statusor = sim_.GNSEnabled();
    ASSERT_TRUE(statusor.ok()) << statusor.status().error_message();
    EXPECT_FALSE(statusor.Value());
}
