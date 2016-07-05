#include <unistd.h>
#include <iostream>
#include <memory>
#include <utility>
#include "external/googletest/googletest/include/gtest/gtest.h"
#include "external/com_github_prattmic_nanopb/util/task/status.h"
#include "io/linux_byteio.h"
#include "io/logging_byteio.h"
#include "log/log.h"
#include "util/status_test.h"

using io::ByteIO;
using io::LinuxByteIO;
using io::LoggingByteIO;

TEST(LoggingByteIOTest, Write) {
    int io_fds[2];
    ASSERT_EQ(0, pipe(io_fds));

    int log_fds[2];
    ASSERT_EQ(0, pipe(log_fds));

    auto log = std::unique_ptr<ByteIO>(new LinuxByteIO(log_fds[1]));
    logging::SetupLogger(std::move(log));

    auto io = std::unique_ptr<ByteIO>(new LinuxByteIO(io_fds[1]));
    auto logging_io = LoggingByteIO(std::move(io));

    auto read_io = LinuxByteIO(io_fds[0]);
    auto read_log = LinuxByteIO(log_fds[0]);

    auto status = logging_io.Write('A');
    ASSERT_OK(status);

    auto statusor = read_io.Read();
    EXPECT_OK(statusor.status());
    EXPECT_EQ('A', statusor.Value());

    const char expected_log[] = "INFO: Writing: 'A'\n";
    for (size_t i = 0; i < sizeof(expected_log)-1; i++) {
        auto statusor = read_log.Read();
        EXPECT_OK(statusor.status());
        EXPECT_EQ(expected_log[i], statusor.Value());
    }
}

TEST(LoggingByteIOTest, Read) {
    int io_fds[2];
    ASSERT_EQ(0, pipe(io_fds));

    int log_fds[2];
    ASSERT_EQ(0, pipe(log_fds));

    auto log = std::unique_ptr<ByteIO>(new LinuxByteIO(log_fds[1]));
    logging::SetupLogger(std::move(log));

    auto io = std::unique_ptr<ByteIO>(new LinuxByteIO(io_fds[0]));
    auto logging_io = LoggingByteIO(std::move(io));

    auto write_io = LinuxByteIO(io_fds[1]);
    auto read_log = LinuxByteIO(log_fds[0]);

    auto status = write_io.Write('A');
    ASSERT_OK(status);

    auto statusor = logging_io.Read();
    EXPECT_OK(statusor.status());
    EXPECT_EQ('A', statusor.Value());

    const char expected_log[] = "INFO: Read: 'A'\n";
    for (size_t i = 0; i < sizeof(expected_log)-1; i++) {
        auto statusor = read_log.Read();
        EXPECT_OK(statusor.status());
        EXPECT_EQ(expected_log[i], statusor.Value());
    }
}
