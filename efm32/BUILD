# Libraries for EFM32-specific features

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "efm32",
    hdrs = [
        "clock.h",
        "gpio.h",
    ],
    srcs = [
        "clock.cc",
        "gpio.cc",
        "peripherals.h",
    ],
    deps = [
        "//log",
        "//third_party/gecko_sdk:efm32hg322f64",
        "@com_github_prattmic_nanopb//util/task:status",
    ],
)

cc_library(
    name = "sim808",
    hdrs = ["sim808.h"],
    srcs = ["sim808.cc"],
    deps = [
        ":efm32",
        "//io:byteio",
        "//log",
        "//sim808",
        "@com_github_prattmic_nanopb//util/task:status",
    ],
)
