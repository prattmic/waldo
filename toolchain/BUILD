package(default_visibility = ["//visibility:public"])

filegroup(
    name = "empty",
    srcs = [],
)

filegroup(
    name = "toolchain",
    srcs = [
        ":cc-compiler-cortex-m0plus",
    ],
)

cc_toolchain(
    name = "cc-compiler-cortex-m0plus",
    all_files = ":empty",
    compiler_files = ":empty",
    cpu = "cortex-m0plus",
    dwp_files = ":empty",
    dynamic_runtime_libs = [":empty"],
    linker_files = ":empty",
    objcopy_files = ":empty",
    static_runtime_libs = [":empty"],
    strip_files = ":empty",
    supports_param_files = 0,
)
