cc_library(
    name = "sim808",
    srcs = [
        "gns.cc",
        "sim808_core.cc",
    ],
    hdrs = [
        "gns.h",
        "sim808.h",
    ],
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
        "//io:logging_byteio",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
    # We need the local serial port.
    tags = ["local"],
)
