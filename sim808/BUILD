cc_library(
    name = "sim808",
    srcs = ["sim808.cc"],
    hdrs = ["sim808.h"],
    deps = [
        "//io:byteio",
        "@nanopb//util/task:status",
    ],
)

cc_test(
    name = "sim808_test",
    srcs = ["sim808_test.cc"],
    deps = [
        ":sim808",
        "//io:linux_byteio",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)
