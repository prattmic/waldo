#include "external/com_github_prattmic_nanopb/util/task/status.h"
#include "external/com_github_prattmic_nanopb/util/task/statusor.h"
#include "io/uart_byteio.h"
#include "log/log.h"
#include "third_party/gecko_sdk/Device/SiliconLabs/EFM32HG/Include/em_device.h"

namespace io {

RingBuffer uart0_rx_buf;
RingBuffer uart1_rx_buf;

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

    NVIC->ISER[0] = 1 << USART1_RX_IRQn;
    usart1->IEN |= USART_IEN_RXDATAV;

    // USART enable
    usart1->CMD = USART_CMD_RXEN | USART_CMD_TXEN;

    return UartByteIO(usart1, &uart1_rx_buf);
}

// TODO(prattmic): deduplicate
util::StatusOr<UartByteIO> UartByteIO::Uart0() {
    // Wait for clock to be ready
    while (!(CMU->STATUS & CMU_STATUS_HFRCORDY));

    CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_USART0 | CMU_HFPERCLKEN0_GPIO;

    USART_TypeDef *usart0 = reinterpret_cast<USART_TypeDef*>(USART0_BASE);

    // Location 0 is RX on PE11 and TX on PE10.
    usart0->ROUTE = USART_ROUTE_RXPEN | USART_ROUTE_TXPEN
                    | USART_ROUTE_LOCATION_LOC0;

    GPIO_TypeDef *gpio = reinterpret_cast<GPIO_TypeDef*>(GPIO_BASE);

    // GPIOs to push-pull output.
    gpio->P[4].MODEH |= GPIO_P_MODEH_MODE10_PUSHPULL
                        | GPIO_P_MODEH_MODE11_PUSHPULL;

    // HFRCO is 14MHz at boot.
    // HFPERCLK is HFRCO/1 at boot.
    // USART br = HFPERCLK / (OVS * (1 + CLKDIV / 256))
    // 115200 = 14MHz / (16 * (1 + CLKDIV/256))
    // 16 * (1 + CLKDIV/256) = 14MHz / 115200
    // CLKDIV = 256 * (14MHz / (115200 * 16) - 1)
    // CLKDIV = 1688
    usart0->CLKDIV = 93077 & _USART_CLKDIV_DIV_MASK;

    NVIC->ISER[0] = 1 << USART0_RX_IRQn;
    usart0->IEN |= USART_IEN_RXDATAV;

    // USART enable
    usart0->CMD = USART_CMD_RXEN | USART_CMD_TXEN;

    return UartByteIO(usart0, &uart0_rx_buf);
}

util::Status UartByteIO::Write(char c) {
    if (!(regs_->STATUS & USART_STATUS_TXBL))
        return util::Status(util::error::Code::RESOURCE_EXHAUSTED, "no space");

    regs_->TXDATA = c;

    return util::Status::OK;
}

util::StatusOr<char> UartByteIO::Read() {
    if (regs_->IF & USART_IF_RXOF) {
        regs_->IFC |= USART_IFC_RXOF;
        LOG(WARNING) << "RX overflow";
    }

    return rx_buf_->Read();
}

}  // namespace io

// Override default interrupt handlers in defined in
// third_party/gecko_sdk/Device/SiliconLabs/EFM32HG/Source/GCC/startup_efm32hg.c.
extern "C" void USART0_RX_IRQHandler() {
    if (USART0->IF & USART_IF_RXDATAV) {
        io::uart0_rx_buf.Write(USART0->RXDATA);
    }
}

extern "C" void USART1_RX_IRQHandler() {
    if (USART1->IF & USART_IF_RXDATAV) {
        io::uart1_rx_buf.Write(USART1->RXDATA);
    }
}
