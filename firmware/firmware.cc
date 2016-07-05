#include <memory>

#include "efm32/clock.h"
#include "efm32/rtc.h"
#include "efm32/sim808.h"
#include "external/com_github_prattmic_nanopb/pb_encode.h"
#include "external/com_github_prattmic_nanopb/util/task/status.h"
#include "firmware/simple.pb.hpp"
#include "http/http.h"
#include "http/sim_http.h"
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

    auto sim = efm32::EFMSIM808(std::move(sim_io));

    LOG(INFO) << "Power cycling SIM808";
    auto status = sim.PowerCycle();
    if (!status.ok()) {
        LOG(ERROR) << "Failed to power cycle SIM808: " << status.error_message();
        return;
    }

    std::unique_ptr<http::Http> http;
    uint8_t response_body[100] = { '\0' };
    size_t size = sizeof(response_body);
    util::StatusOr<http::HTTPResponse> statusorresp;
    http::HTTPResponse resp;

    LOG(INFO) << "Initializing SIM808";
    status = sim.Initialize();
    if (!status.ok()) {
        LOG(ERROR) << "Failed to initialize SIM808: " << status.error_message();
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

    http = std::unique_ptr<http::Http>(new http::SIMHttp(&sim));
    LOG(INFO) << "HTTP GET http://pratt.im/hello.txt";

    statusorresp = http->Get("http://pratt.im/hello.txt", response_body, size);
    if (!statusorresp.ok()) {
        LOG(ERROR) << "Failed to HTTP GET: "
                   << statusorresp.status().error_message();
        goto out_disable_http;
    }

    resp = statusorresp.Value();
    if (resp.status_code != 200) {
        LOG(ERROR) << "Bad response code; expected 200 got "
                   << resp.status_code;
        goto out_disable_http;
    }

    LOG(INFO) << "Response body length: "
              << resp.body_length;

    if (resp.body_length != resp.copied_length) {
        LOG(WARNING) << "Response body length " << resp.body_length
                     << " bytes, but only read " << resp.copied_length
                     << " bytes";
    }

    LOG(INFO) << "Response body: " << response_body;

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
    auto status = efm32::ClockInit();
    if (!status.ok())
        return;

    status = efm32::RTCInit();
    if (!status.ok())
        return;

    SetupLogging();

    LOG(INFO) << "Waldo booted!";

    HTTP();

    LOG(INFO) << "Waldo done!";
}

}  // namespace waldo

extern "C" int main(void) {
    waldo::Main();
    while (1) {}
    return 0;
}
