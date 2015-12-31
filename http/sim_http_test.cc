#include <fcntl.h>
#include <unistd.h>
#include <memory>
#include <utility>
#include "external/googletest/googletest/include/gtest/gtest.h"
#include "external/nanopb/util/task/status.h"
#include "http/http.h"
#include "http/sim_http.h"
#include "io/linux_byteio.h"
#include "io/logging_byteio.h"
#include "log/log.h"
#include "sim808/sim808.h"
#include "util/status_test.h"

using http::Http;
using http::SIMHttp;
using io::ByteIO;
using io::LinuxByteIO;
using io::LoggingByteIO;
using sim808::SIM808;

class SIMHttpTest : public ::testing::Test {
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

        auto status = sim_.Initialize();
        ASSERT_OK(status);

        // Try to disable. These may return an error already disabled.
        sim_.HTTPEnable(false);
        sim_.GPRSEnable(false);

        status = sim_.GPRSEnable(true);
        ASSERT_OK(status);

        status = sim_.HTTPEnable(true);
        ASSERT_OK(status);

        http_ = std::unique_ptr<Http>(new SIMHttp(&sim_));
    }

    virtual void TearDown() {
        auto status = sim_.HTTPEnable(false);
        EXPECT_OK(status);

        status = sim_.GPRSEnable(false);
        EXPECT_OK(status);
    }

    SIM808 sim_;
    std::unique_ptr<Http> http_;
};

TEST_F(SIMHttpTest, Get) {
    char response_body[100] = { '\0' };
    size_t size = sizeof(response_body);

    // Returns "hello\nworld\n".
    auto statusor = http_->Get("http://pratt.im/hello.txt", response_body,
                               size);
    ASSERT_OK(statusor.status());

    auto resp_status = statusor.Value();
    EXPECT_EQ(200, resp_status.status_code);
    EXPECT_EQ(12ull, resp_status.body_length);
    EXPECT_EQ(12ull, resp_status.copied_length);

    EXPECT_STREQ("hello\nworld\n", response_body);
}

TEST_F(SIMHttpTest, HTTPPost) {
    uint8_t message[] = { 'h', 'i' };
    size_t size = 2;

    char response_body[100] = { '\0' };

    // Response body contains the Content-Length of the request.
    auto statusor = http_->Post("http://pratt.im/post", message, size,
                                response_body, sizeof(response_body));
    ASSERT_OK(statusor.status());
    auto resp_status = statusor.Value();
    EXPECT_EQ(200, resp_status.status_code);
    EXPECT_EQ(1ull, resp_status.body_length);
    EXPECT_EQ(1ull, resp_status.copied_length);

    EXPECT_STREQ("2", response_body);
}
