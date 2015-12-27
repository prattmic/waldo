package(default_visibility = ["//visibility:public"])

cc_library(
    name = "string",
    srcs = ["string.cc"],
    hdrs = ["string.h"],
)

cc_library(
    name = "status_test",
    hdrs = ["status_test.h"],
    deps = [
        "@googletest//:gtest",
        "@googletest//:gmock",
        "@nanopb//util/task:status",
    ],
    testonly = 1,
)
