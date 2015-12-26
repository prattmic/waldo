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
        int fd = open("/tmp/dummy.txt", O_CREAT|O_RDONLY, 0644);
        ASSERT_GE(fd, 0);
        ASSERT_EQ(0, close(fd));

        auto statusor = LinuxByteIO::OpenFile("/tmp/dummy.txt");
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
    // TODO: check return
    sim_.Initialize();
}
