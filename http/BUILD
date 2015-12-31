package(default_visibility = ["//visibility:public"])

cc_library(
    name = "cpr_http",
    hdrs = ["cpr_http.h"],
    deps = [
        "@cpr//:cpr",
        "@nanopb//util/task:status",
    ],
)

cc_test(
    name = "cpr_http_test",
    srcs = ["cpr_http_test.cc"],
    deps = [
        ":cpr_http",
        "//util:status_test",
        "@googletest//:gtest",
        "@googletest//:gmock_main",
    ],
    # We need network access.
    tags = ["local"],
)
