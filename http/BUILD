package(default_visibility = ["//visibility:public"])

cc_library(
    name = "http",
    hdrs = ["http.h"],
    deps = [
        "@nanopb//util/task:status",
    ],
)

cc_library(
    name = "cpr_http",
    hdrs = ["cpr_http.h"],
    deps = [
        ":http",
        "@cpr//:cpr",
        "@nanopb//util/task:status",
    ],
)

cc_test(
    name = "cpr_http_test",
    srcs = ["cpr_http_test.cc"],
    deps = [
        ":cpr_http",
        ":http",
        "//util:status_test",
        "@googletest//:gtest",
        "@googletest//:gmock_main",
    ],
    # We need network access.
    tags = ["local"],
)

cc_library(
    name = "sim_http",
    hdrs = ["sim_http.h"],
    deps = [
        ":http",
        "//sim808",
        "@nanopb//util/task:status",
    ],
)

cc_test(
    name = "sim_http_test",
    srcs = ["sim_http_test.cc"],
    deps = [
        ":http",
        ":sim_http",
        "//io:byteio",
        "//io:linux_byteio",
        "//io:logging_byteio",
        "//log",
        "//sim808",
        "//util:status_test",
        "@googletest//:gtest",
        "@googletest//:gmock_main",
    ],
    # We need serial port access.
    tags = ["local"],
)
