cc_library(
    name = "log",
    srcs = ["log.cc"],
    hdrs = ["log.h"],
    deps = [
        "//io:byteio",
    ],
)

cc_test(
    name = "log_test",
    srcs = ["log_test.cc"],
    deps = [
        ":log",
        "//io:byteio",
        "//io:linux_byteio",
        "//util:status_test",
        "@googletest//:gtest",
        "@googletest//:gmock_main",
    ],
)
