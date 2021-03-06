licenses(["notice"])  # zlib license

package(
    default_visibility = ["//visibility:public"],
)

cc_library(
    name = "efm32hg322f64",
    defines = ["EFM32HG322F64"],
    hdrs = [
        "Device/SiliconLabs/EFM32HG/Include/em_device.h",
    ],
    srcs = [
        "Device/SiliconLabs/EFM32HG/Source/system_efm32hg.c",
        "Device/SiliconLabs/EFM32HG/Include/efm32hg322f64.h",
        "Device/SiliconLabs/EFM32HG/Include/efm32hg_acmp.h",
        "Device/SiliconLabs/EFM32HG/Include/efm32hg_adc.h",
        "Device/SiliconLabs/EFM32HG/Include/efm32hg_aes.h",
        "Device/SiliconLabs/EFM32HG/Include/efm32hg_af_pins.h",
        "Device/SiliconLabs/EFM32HG/Include/efm32hg_af_ports.h",
        "Device/SiliconLabs/EFM32HG/Include/efm32hg_calibrate.h",
        "Device/SiliconLabs/EFM32HG/Include/efm32hg_cmu.h",
        "Device/SiliconLabs/EFM32HG/Include/efm32hg_devinfo.h",
        "Device/SiliconLabs/EFM32HG/Include/efm32hg_dma_ch.h",
        "Device/SiliconLabs/EFM32HG/Include/efm32hg_dmactrl.h",
        "Device/SiliconLabs/EFM32HG/Include/efm32hg_dma_descriptor.h",
        "Device/SiliconLabs/EFM32HG/Include/efm32hg_dma.h",
        "Device/SiliconLabs/EFM32HG/Include/efm32hg_dmareq.h",
        "Device/SiliconLabs/EFM32HG/Include/efm32hg_emu.h",
        "Device/SiliconLabs/EFM32HG/Include/efm32hg_gpio.h",
        "Device/SiliconLabs/EFM32HG/Include/efm32hg_gpio_p.h",
        "Device/SiliconLabs/EFM32HG/Include/efm32hg_i2c.h",
        "Device/SiliconLabs/EFM32HG/Include/efm32hg_idac.h",
        "Device/SiliconLabs/EFM32HG/Include/efm32hg_leuart.h",
        "Device/SiliconLabs/EFM32HG/Include/efm32hg_msc.h",
        "Device/SiliconLabs/EFM32HG/Include/efm32hg_mtb.h",
        "Device/SiliconLabs/EFM32HG/Include/efm32hg_pcnt.h",
        "Device/SiliconLabs/EFM32HG/Include/efm32hg_prs_ch.h",
        "Device/SiliconLabs/EFM32HG/Include/efm32hg_prs.h",
        "Device/SiliconLabs/EFM32HG/Include/efm32hg_prs_signals.h",
        "Device/SiliconLabs/EFM32HG/Include/efm32hg_rmu.h",
        "Device/SiliconLabs/EFM32HG/Include/efm32hg_romtable.h",
        "Device/SiliconLabs/EFM32HG/Include/efm32hg_rtc.h",
        "Device/SiliconLabs/EFM32HG/Include/efm32hg_timer_cc.h",
        "Device/SiliconLabs/EFM32HG/Include/efm32hg_timer.h",
        "Device/SiliconLabs/EFM32HG/Include/efm32hg_usart.h",
        "Device/SiliconLabs/EFM32HG/Include/efm32hg_usb_diep.h",
        "Device/SiliconLabs/EFM32HG/Include/efm32hg_usb_doep.h",
        "Device/SiliconLabs/EFM32HG/Include/efm32hg_usb.h",
        "Device/SiliconLabs/EFM32HG/Include/efm32hg_vcmp.h",
        "Device/SiliconLabs/EFM32HG/Include/efm32hg_wdog.h",
        "Device/SiliconLabs/EFM32HG/Include/system_efm32hg.h",
    ],
    deps = [
        "//third_party/CMSIS:includes",
    ],
)

cc_library(
    name = "startup",
    srcs = [
        "Device/SiliconLabs/EFM32HG/Source/GCC/startup_efm32hg.c",
    ],
    deps = [":efm32hg322f64"],
    alwayslink = 1,
    # Startup code must *not* use the GOT, or anything else in the .data
    # section, since it is responsible for setting up the .data section.
    linkstatic = 1,
    nocopts = "-fPIC",
)

exports_files(["Device/SiliconLabs/EFM32HG/Source/GCC/efm32hg.ld"])
