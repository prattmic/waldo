#include "external/googletest/googletest/include/gtest/gtest.h"
#include "http/http.h"
#include "http/cpr_http.h"
#include "proto/location/location.pb.hpp"
#include "util/status_test.h"

TEST(LocationTest, Post) {
    Location l;
    l.set_latitude("36.4578");
    l.set_longitude("-76.3276");
    l.set_altitude_msl("100.5");
    l.set_ground_speed("5.7");
    l.set_heading("25.6");

    uint8_t proto[Location_size] = { 0 };

    auto serialize_status = l.Serialize(proto, Location_size);
    ASSERT_OK(serialize_status.status());
    size_t serialized_bytes = serialize_status.Value();

    auto http = std::unique_ptr<http::Http>(new http::CPRHttp());

    uint8_t response_body[Location_size] = { 0 };
    auto post_status = http->Post("http://localhost:8080/echo", proto,
                                  serialized_bytes, response_body,
                                  Location_size);
    ASSERT_OK(post_status.status());

    auto response = post_status.Value();
    EXPECT_EQ(200, response.status_code);
    EXPECT_EQ(response.copied_length, response.body_length);

    auto deserialize_status = Location::Deserialize(response_body,
                                                    response.copied_length);
    ASSERT_OK(deserialize_status.status());

    auto echoed = deserialize_status.Value();
    EXPECT_STREQ(l.get_latitude(), echoed.get_latitude());
    EXPECT_STREQ(l.get_longitude(), echoed.get_longitude());
    EXPECT_STREQ(l.get_altitude_msl(), echoed.get_altitude_msl());
    EXPECT_STREQ(l.get_ground_speed(), echoed.get_ground_speed());
    EXPECT_STREQ(l.get_heading(), echoed.get_heading());
}
