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
        "//util:status_test",
        "@googletest//:gtest",
        "@googletest//:gmock_main",
    ],
)

cc_library(
    name = "logging_byteio",
    hdrs = ["logging_byteio.h"],
    deps = [
        ":byteio",
        "//util:string",
        "@nanopb//util/task:status",
    ],
)

cc_test(
    name = "logging_byteio_test",
    srcs = ["logging_byteio_test.cc"],
    deps = [
        ":linux_byteio",
        ":logging_byteio",
        "//util:status_test",
        "@googletest//:gtest",
        "@googletest//:gmock_main",
    ],
)

cc_library(
    name = "byteio_streambuf",
    hdrs = ["byteio_streambuf.h"],
    deps = [
        ":byteio",
    ],
)

cc_test(
    name = "byteio_streambuf_test",
    srcs = ["byteio_streambuf_test.cc"],
    deps = [
        ":byteio_streambuf",
        ":linux_byteio",
        "//util:status_test",
        "@googletest//:gtest",
        "@googletest//:gmock_main",
    ],
)
