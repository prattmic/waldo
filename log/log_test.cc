#include <unistd.h>
#include <memory>
#include <utility>
#include "external/googletest/googletest/include/gtest/gtest.h"
#include "io/linux_byteio.h"
#include "log/log.h"
#include "util/status_test.h"

using io::ByteIO;
using io::LinuxByteIO;

TEST(LogTest, String) {
    int io_fds[2];
    ASSERT_EQ(0, pipe(io_fds));

    auto io = std::unique_ptr<ByteIO>(new LinuxByteIO(io_fds[1]));
    logging::SetupLogger(std::move(io));

    LOG(INFO) << "Hello" << " World";

    auto read_io = LinuxByteIO(io_fds[0]);

    const char expected_log[] = "INFO: Hello World\n";
    for (size_t i = 0; i < sizeof(expected_log)-1; i++) {
        auto statusor = read_io.Read();
        EXPECT_OK(statusor.status());
        EXPECT_EQ(expected_log[i], statusor.Value());
    }
}
