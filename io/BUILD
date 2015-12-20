package(default_visibility = ["//visibility:public"])

cc_library(
    name = "byteio",
    hdrs = ["byteio.h"],
    deps = ["@nanopb//util/task:status"],
)

cc_library(
    name = "linux_byteio",
    srcs = ["linux_byteio.cc"],
    hdrs = ["linux_byteio.h"],
    deps = [
        ":byteio",
        "@nanopb//util/task:status",
    ],
)

cc_test(
    name = "linux_byteio_test",
    srcs = ["linux_byteio_test.cc"],
    deps = [
        ":linux_byteio",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)
