load("@com_github_prattmic_nanopb//:nanopb.bzl", "nanopb_cpp_library")
load("//third_party/gecko_sdk:binary.bzl", "efm32_binary")
load("//tools:raw_binary.bzl", "raw_binary")

nanopb_cpp_library(
    name = "simple",
    proto = "simple.proto",
)

efm32_binary(
    name = "firmware",
    srcs = ["firmware.cc"],
    deps = [
        ":simple",
        "//efm32:sim808",
        "//http",
        "//http:sim_http",
        "//io:byteio",
        "//io:uart_byteio",
        "//log",
        "@com_github_prattmic_nanopb//util/task:status",
    ],
)

raw_binary(
    name = "bin",
    src = ":firmware",
)

sh_binary(
    name = "flash",
    srcs = ["flash.sh"],
    data = [":bin.bin"],
)
