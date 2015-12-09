#include "external/googletest/googletest/include/gtest/gtest.h"

TEST(CommTest, Success) {
    EXPECT_TRUE(true);
}

TEST(CommTest, Fail) {
    EXPECT_TRUE(false);
}
