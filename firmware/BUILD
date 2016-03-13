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
        "//io:null_byteio",
        "//io:uart_byteio",
        "//sim808",
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
