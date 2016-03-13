#include "external/nanopb/util/task/status.h"
#include "external/nanopb/util/task/statusor.h"
#include "io/uart_byteio.h"
#include "third_party/gecko_sdk/Device/SiliconLabs/EFM32HG/Include/em_device.h"

using io::UartByteIO;

util::StatusOr<UartByteIO> UartByteIO::Uart1() {
    // TODO(prattmic): some of this clock work should be moved.

    // Wait for clock to be ready
    while (!(CMU->STATUS & CMU_STATUS_HFRCORDY));

    CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_USART1 | CMU_HFPERCLKEN0_GPIO;

    USART_TypeDef *usart1 = reinterpret_cast<USART_TypeDef*>(USART1_BASE);

    // Location 2 is RX on PD6 and TX on PD7.
    usart1->ROUTE = USART_ROUTE_RXPEN | USART_ROUTE_TXPEN
                    | USART_ROUTE_LOCATION_LOC2;

    GPIO_TypeDef *gpio = reinterpret_cast<GPIO_TypeDef*>(GPIO_BASE);

    // GPIOs to push-pull output.
    gpio->P[3].MODEL |= GPIO_P_MODEL_MODE6_PUSHPULL
                        | GPIO_P_MODEL_MODE7_PUSHPULL;

    // HFRCO is 14MHz at boot.
    // HFPERCLK is HFRCO/1 at boot.
    // USART br = HFPERCLK / (OVS * (1 + CLKDIV / 256))
    // 115200 = 14MHz / (16 * (1 + CLKDIV/256))
    // 16 * (1 + CLKDIV/256) = 14MHz / 115200
    // CLKDIV = 256 * (14MHz / (115200 * 16) - 1)
    // CLKDIV = 1688
    usart1->CLKDIV = 1688 & _USART_CLKDIV_DIV_MASK;

    // USART enable
    usart1->CMD = USART_CMD_RXEN | USART_CMD_TXEN;

    return UartByteIO(usart1);
}

util::Status UartByteIO::Write(char c) {
    while (!(regs_->STATUS & USART_STATUS_TXBL));

    regs_->TXDATA = c;

    return util::Status::OK;
}

util::StatusOr<char> UartByteIO::Read() {
    return util::Status(util::error::Code::UNIMPLEMENTED, "TODO");
}
