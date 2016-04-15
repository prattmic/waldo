#include <memory>

#include "external/nanopb/pb_encode.h"
#include "external/nanopb/util/task/status.h"
#include "firmware/simple.pb.hpp"
#include "io/byteio.h"
#include "io/uart_byteio.h"
#include "log/log.h"
#include "sim808/sim808.h"

namespace waldo {

void HTTP() {
    auto statusor = io::UartByteIO::Uart0();
    if (!statusor.ok()) {
        while (1) {}
    }

    auto uart_io = statusor.ConsumeValue();
    std::unique_ptr<io::ByteIO> sim_io(new io::UartByteIO(std::move(uart_io)));

    auto sim = sim808::SIM808(std::move(sim_io));

    uint8_t buf[13] = { '\0' };
    ::util::StatusOr<::sim808::HTTPResponseStatus> resp;
    ::util::StatusOr<size_t> read;

    LOG(INFO) << "Initializing SIM808";
    auto status = sim.Initialize();
    if (!status.ok()) {
        LOG(ERROR) << "Failed to initialize SIM808: " << status.ToString();
        return;
    }

    status = sim.GPRSEnable(true);
    if (!status.ok()) {
        LOG(ERROR) << "Failed to enable GPRS: " << status.error_message();
        return;
    }

    status = sim.HTTPEnable(true);
    if (!status.ok()) {
        LOG(ERROR) << "Failed to enable HTTP: " << status.error_message();
        goto out_disable_gprs;
    }

    LOG(INFO) << "HTTP GET http://pratt.im/hello.txt";

    resp = sim.HTTPGet("http://pratt.im/hello.txt");
    if (!resp.ok()) {
        LOG(ERROR) << "Failed to HTTP GET: " << resp.status().error_message();
        goto out_disable_http;
    } else if (resp.Value().code != 200) {
        LOG(ERROR) << "Bad response code; expected 200 got " << static_cast<uint32_t>(resp.Value().code);
        goto out_disable_http;
    }

    LOG(INFO) << "Response size: " << static_cast<uint32_t>(resp.Value().bytes);

    read = sim.HTTPRead(buf, 12);
    if (!read.ok()) {
        LOG(ERROR) << "Failed to read response: " << read.status().error_message();
        goto out_disable_http;
    }

    LOG(INFO) << "Read " << static_cast<uint32_t>(read.Value()) << " bytes";
    LOG(INFO) << "Response body: " << reinterpret_cast<const char*>(buf);

out_disable_http:
    status = sim.HTTPEnable(false);
    if (!status.ok())
        LOG(ERROR) << "Failed to disable HTTP: " << status.error_message();

out_disable_gprs:
    status = sim.GPRSEnable(false);
    if (!status.ok())
        LOG(ERROR) << "Failed to disable GPRS: " << status.error_message();
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

// Initialize the logging UART and logger.
void SetupLogging() {
    auto statusor = io::UartByteIO::Uart1();
    if (!statusor.ok()) {
        while (1) {}
    }

    auto uart_io = statusor.ConsumeValue();
    std::unique_ptr<io::ByteIO> io(new io::UartByteIO(std::move(uart_io)));
    logging::SetupLogger(std::move(io));
}

void Main() {
    SetupLogging();

    LOG(INFO) << "Waldo booted!";

    HTTP();

    while (1) {
        //LOG(INFO) << "Hello" << " World!";
    }
}

}  // namespace waldo

extern "C" int main(void) {
    waldo::Main();
    return 0;
}
