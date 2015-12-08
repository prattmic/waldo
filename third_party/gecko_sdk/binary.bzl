def efm32_binary(name, deps=[], copts=[], linkopts=[], **kwargs):
    """Create an EFM32 cc_binary.

    Automatically uses an EFM32 linker script and links against startup code.
    """
    native.cc_binary(
        name = name,
        deps = deps + [
            "//third_party/gecko_sdk:startup",
            "//third_party/gecko_sdk:Device/SiliconLabs/EFM32HG/Source/GCC/efm32hg.ld",
        ],
        copts = copts + [
            "-nostdlib",
            "-ffreestanding",
        ],
        linkopts = [
            "-static",
            "-nostdlib",
            "-T",
            "//third_party/gecko_sdk:Device/SiliconLabs/EFM32HG/Source/GCC/efm32hg.ld",
        ],
        linkstatic = 1,
        **kwargs
    )
