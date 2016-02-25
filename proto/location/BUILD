load("//tools:nanopb.bzl", "nanopb_cpp_library")

nanopb_cpp_library(
    name = "location",
    proto = "location.proto",
    options = "location.options",
)

cc_test(
    name = "location_test",
    srcs = ["location_test.cc"],
    deps = [
        ":location",
        "//http",
        "//http:cpr_http",
        "//util:status_test",
        "@googletest//:gtest",
        "@googletest//:gmock_main",
    ],
    # Need network access
    tags = ["local"],
)
