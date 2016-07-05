package(
    default_visibility = ["//visibility:public"],
)

cc_library(
    name = "bsp",
    srcs = ["bsp.c"],
    alwayslink = 1,
)

cc_library(
    name = "efm32",
    srcs = ["efm32.cc"],
    deps = [
        ":bsp",
        "//efm32",
    ],
    alwayslink = 1,
)
