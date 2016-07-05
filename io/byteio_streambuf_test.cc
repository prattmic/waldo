#include <unistd.h>
#include <iostream>
#include <memory>
#include <utility>
#include "external/googletest/googletest/include/gtest/gtest.h"
#include "external/com_github_prattmic_nanopb/util/task/status.h"
#include "io/byteio_streambuf.h"
#include "io/linux_byteio.h"
#include "util/status_test.h"

using io::ByteIO;
using io::LinuxByteIO;
using io::ByteIOStreamBuf;

TEST(ByteIOStreamBuf, Write) {
    int io_fds[2];
    ASSERT_EQ(0, pipe(io_fds));

    auto io = std::unique_ptr<ByteIO>(new LinuxByteIO(io_fds[1]));
    auto streambuf = ByteIOStreamBuf(std::move(io));
    std::ostream os(static_cast<std::streambuf*>(&streambuf));

    os << "Hello\n";
    os << 123;

    auto read_io = LinuxByteIO(io_fds[0]);

    const char expected[] = "Hello\n123";
    for (size_t i = 0; i < sizeof(expected)-1; i++) {
        auto statusor = read_io.Read();
        EXPECT_OK(statusor.status());
        EXPECT_EQ(expected[i], statusor.Value());
    }
}
