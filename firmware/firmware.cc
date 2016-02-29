#include <memory>

#include "external/nanopb/pb_encode.h"
#include "firmware/simple.pb.hpp"
#include "io/byteio.h"
#include "io/null_byteio.h"
#include "sim808/sim808.h"
#include "third_party/gecko_sdk/Device/SiliconLabs/EFM32HG/Include/em_device.h"

namespace waldo {

void HTTP() {
    std::unique_ptr<io::ByteIO> io(new io::NullByteIO());

    auto sim = sim808::SIM808(std::move(io));

    sim.GPRSEnable(true);
    sim.HTTPEnable(true);
    sim.HTTPGet("http://pratt.im/hello.txt");

    uint8_t buf[13] = { '\0' };

    sim.HTTPRead(buf, 12);
}

void RandomTest() {
    auto a = std::make_unique<void*>(nullptr);
    Simple s;
    s.set_double_field(5.0);
    uint8_t buf[Simple_size];
    s.Serialize(buf, Simple_size);
    s.Deserialize(buf, Simple_size);

    HTTP();
}

void UARTInit() {
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
}

void UARTWrite(char c) {
    USART_TypeDef *usart1 = reinterpret_cast<USART_TypeDef*>(USART1_BASE);

    while (!(usart1->STATUS & USART_STATUS_TXBL));

    usart1->TXDATA = c;
}

void Main() {
    UARTInit();

    while (1) {
        UARTWrite('H');
        UARTWrite('e');
        UARTWrite('l');
        UARTWrite('l');
        UARTWrite('o');
        UARTWrite('\r');
        UARTWrite('\n');
    }
}

}  // namespace waldo

extern "C" int main(void) {
    waldo::Main();
    return 0;
}
