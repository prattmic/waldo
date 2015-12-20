#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <memory>
#include <utility>
#include "external/googletest/googletest/include/gtest/gtest.h"
#include "external/nanopb/util/task/status.h"
#include "io/linux_byteio.h"
#include "sim808/sim808.h"

using io::LinuxByteIO;
using sim808::SIM808;

TEST(SIM808Test, Connect) {
    int fd = open("/tmp/log.txt", O_CREAT|O_RDONLY, 0644);
    ASSERT_GE(fd, 0);
    ASSERT_EQ(0, close(fd));

    auto statusor = LinuxByteIO::OpenFile("/tmp/log.txt");
    ASSERT_TRUE(statusor.ok()) << statusor.status().ToString();

    auto io = std::unique_ptr<io::ByteIO>(
        new LinuxByteIO(statusor.ConsumeValue()));

    auto sim = SIM808(std::move(io));
    sim.Initialize();
}
