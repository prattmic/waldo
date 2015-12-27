#ifndef UTIL_STATUS_TEST_H_
#define UTIL_STATUS_TEST_H_

#include <iostream>
#include "external/googletest/googlemock/include/gmock/gmock.h"
#include "external/googletest/googletest/include/gtest/gtest.h"
#include "external/nanopb/util/task/status.h"

namespace testing {

class IsOKMatcher : public MatcherInterface<::util::Status> {
 public:
    virtual bool MatchAndExplain(::util::Status status,
                                 MatchResultListener* listener) const {
        bool ok = status.ok();
        if (!ok) {
            *listener << "error code: " << status.error_code() << ", "
                      << "error message: " << status.error_message();
        }

        return ok;
    }

    virtual void DescribeTo(::std::ostream* os) const {
        *os << "is OK";
    }

    virtual void DescribeNegationTo(::std::ostream* os) const {
        *os << "is not OK";
    }
};

inline Matcher<::util::Status> IsOK() {
    return MakeMatcher(new IsOKMatcher);
}

}  // namespace testing

#define EXPECT_OK(_p)       EXPECT_THAT(_p, ::testing::IsOK())
#define ASSERT_OK(_p)       ASSERT_THAT(_p, ::testing::IsOK())
#define EXPECT_NOT_OK(_p)   EXPECT_THAT(_p, ::testing::Not(::testing::IsOK()))
#define ASSERT_NOT_OK(_p)   ASSERT_THAT(_p, ::testing::Not(::testing::IsOK()))

#endif  // UTIL_STATUS_TEST_H_
