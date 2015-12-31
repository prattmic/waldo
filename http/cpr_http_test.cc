#include <memory>
#include "external/googletest/googletest/include/gtest/gtest.h"
#include "http/cpr_http.h"
#include "http/http.h"
#include "util/status_test.h"

using http::Http;
using http::CPRHttp;

TEST(CPRHttpTest, Get) {
    auto http = std::unique_ptr<Http>(new CPRHttp());

    char body[100] = { '\0' };

    auto statusor = http->Get("http://pratt.im/hello.txt", body, sizeof(body));
    EXPECT_OK(statusor.status());

    auto response = statusor.Value();
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(11ull, response.body_length);
    EXPECT_EQ(11ull, response.copied_length);

    // For some reason CPR strips the trailing newline.
    EXPECT_STREQ("hello\nworld", body);
}

TEST(CPRHttpTest, Post) {
    auto http = std::unique_ptr<Http>(new CPRHttp());

    uint8_t data[] = "hi";

    char body[100] = { '\0' };

    auto statusor = http->Post("http://pratt.im/post", data, sizeof(data),
                              body, sizeof(body));
    EXPECT_OK(statusor.status());

    auto response = statusor.Value();
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(1ull, response.body_length);
    EXPECT_EQ(1ull, response.copied_length);

    // CPR includes the trailing NUL.
    EXPECT_STREQ("3", body);
}
