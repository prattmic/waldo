load("//third_party/gecko_sdk:binary.bzl", "efm32_binary")
load("//tools:nanopb.bzl", "nanopb_cpp_library")
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
        "//io:byteio",
        "//io:uart_byteio",
        "//log",
        "//sim808",
        "@nanopb//util/task:status",
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
