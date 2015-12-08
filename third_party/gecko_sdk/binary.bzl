def efm32_binary(name, deps=[], copts=[], linkopts=[], **kwargs):
    """Create an EFM32 cc_binary.

    Automatically uses an EFM32 linker script and links against startup code
    and a board support package for newlib.
    """
    native.cc_binary(
        name = name,
        deps = deps + [
            "//bsp",
            "//third_party/gecko_sdk:startup",
            "//third_party/gecko_sdk:Device/SiliconLabs/EFM32HG/Source/GCC/efm32hg.ld",
        ],
        linkopts = [
            "-static",
            "-T",
            "//third_party/gecko_sdk:Device/SiliconLabs/EFM32HG/Source/GCC/efm32hg.ld",
        ],
        linkstatic = 1,
        **kwargs
    )
