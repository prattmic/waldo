load("/third_party/gecko_sdk/binary", "efm32_binary")
load("/tools/nanopb", "nanopb_cpp_library")
load("/tools/raw_binary", "raw_binary")

nanopb_cpp_library(
    name = "simple",
    proto = "simple.proto",
)

efm32_binary(
    name = "firmware",
    srcs = ["firmware.cc"],
    deps = [":simple"],
)

raw_binary(
    name = "bin",
    src = ":firmware",
)
