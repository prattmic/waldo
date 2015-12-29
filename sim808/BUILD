package(default_visibility = ["//visibility:public"])

cc_library(
    name = "sim808",
    srcs = [
        "gns.cc",
        "http.cc",
        "sim808_core.cc",
    ],
    hdrs = [
        "gns.h",
        "http.h",
        "sim808.h",
    ],
    deps = [
        "//io:byteio",
        "//log",
        "@nanopb//util/task:status",
    ],
)

cc_test(
    name = "sim808_test",
    srcs = ["sim808_test.cc"],
    deps = [
        ":sim808",
        "//io:linux_byteio",
        "//io:logging_byteio",
        "//log",
        "//util:status_test",
        "@googletest//:gtest",
        "@googletest//:gmock_main",
    ],
    # We need the local serial port.
    tags = ["local"],
)
