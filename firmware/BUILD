load("/third_party/gecko_sdk/binary", "efm32_binary")
load("/tools/raw_binary", "raw_binary")

efm32_binary(
    name = "firmware",
    srcs = ["firmware.c"],
)

raw_binary(
    name = "bin",
    src = ":firmware",
)
