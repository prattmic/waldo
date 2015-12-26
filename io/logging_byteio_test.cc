#include <unistd.h>
#include <iostream>
#include <memory>
#include <utility>
#include "external/googletest/googletest/include/gtest/gtest.h"
#include "external/nanopb/util/task/status.h"
#include "io/linux_byteio.h"
#include "io/logging_byteio.h"

using io::ByteIO;
using io::LinuxByteIO;
using io::LoggingByteIO;

TEST(LoggingByteIOTest, Write) {
    int io_fds[2];
    ASSERT_EQ(0, pipe(io_fds));

    int log_fds[2];
    ASSERT_EQ(0, pipe(log_fds));

    auto io = std::unique_ptr<ByteIO>(new LinuxByteIO(io_fds[1]));
    auto log = std::unique_ptr<ByteIO>(new LinuxByteIO(log_fds[1]));
    auto logging_io = LoggingByteIO(std::move(io), std::move(log));

    auto read_io = LinuxByteIO(io_fds[0]);
    auto read_log = LinuxByteIO(log_fds[0]);

    auto status = logging_io.Write('A');
    ASSERT_TRUE(status.ok()) << status.error_message();

    auto statusor = read_io.Read();
    EXPECT_TRUE(statusor.ok()) << statusor.status().error_message();
    EXPECT_EQ('A', statusor.Value());

    const char expected_log[] = "Writing: 'A'\n";
    for (size_t i = 0; i < sizeof(expected_log)-1; i++) {
        auto statusor = read_log.Read();
        EXPECT_TRUE(statusor.ok()) << statusor.status().error_message();
        EXPECT_EQ(expected_log[i], statusor.Value());
    }
}

TEST(LoggingByteIOTest, Read) {
    int io_fds[2];
    ASSERT_EQ(0, pipe(io_fds));

    int log_fds[2];
    ASSERT_EQ(0, pipe(log_fds));

    auto io = std::unique_ptr<ByteIO>(new LinuxByteIO(io_fds[0]));
    auto log = std::unique_ptr<ByteIO>(new LinuxByteIO(log_fds[1]));
    auto logging_io = LoggingByteIO(std::move(io), std::move(log));

    auto write_io = LinuxByteIO(io_fds[1]);
    auto read_log = LinuxByteIO(log_fds[0]);

    auto status = write_io.Write('A');
    ASSERT_TRUE(status.ok()) << status.error_message();

    auto statusor = logging_io.Read();
    EXPECT_TRUE(statusor.ok()) << statusor.status().error_message();
    EXPECT_EQ('A', statusor.Value());

    const char expected_log[] = "Read: 'A'\n";
    for (size_t i = 0; i < sizeof(expected_log)-1; i++) {
        auto statusor = read_log.Read();
        EXPECT_TRUE(statusor.ok()) << statusor.status().error_message();
        EXPECT_EQ(expected_log[i], statusor.Value());
    }
}
