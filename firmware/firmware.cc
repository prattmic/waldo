#include <memory>

#include "external/nanopb/pb_encode.h"
#include "firmware/simple.pb.hpp"
#include "io/byteio.h"
#include "io/null_byteio.h"
#include "io/uart_byteio.h"
#include "sim808/sim808.h"

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

void Main() {
    auto statusor = io::UartByteIO::Uart1();
    if (!statusor.ok()) {
        while (1) {
        }
    }

    auto io = statusor.ConsumeValue();

    while (1) {
        io.Write('H');
        io.Write('e');
        io.Write('l');
        io.Write('l');
        io.Write('o');
        io.Write('\r');
        io.Write('\n');
    }
}

}  // namespace waldo

extern "C" int main(void) {
    waldo::Main();
    return 0;
}
