#ifndef IO_UART_BYTEIO_H_
#define IO_UART_BYTEIO_H_

#include <unistd.h>
#include "external/com_github_prattmic_nanopb/util/task/status.h"
#include "external/com_github_prattmic_nanopb/util/task/statusor.h"
#include "io/byteio.h"
#include "third_party/gecko_sdk/Device/SiliconLabs/EFM32HG/Include/em_device.h"

namespace io {

class UartByteIO : public ByteIO {
 public:
    static ::util::StatusOr<UartByteIO> Uart0();
    static ::util::StatusOr<UartByteIO> Uart1();

    // Read a single byte.
    virtual ::util::StatusOr<char> Read() override;

    // Write a single byte.
    virtual ::util::Status Write(char c) override;

    // For StatusOr.
    UartByteIO() {};

    // Move constructor.
    UartByteIO(UartByteIO&& other) : regs_(other.regs_) {
        other.regs_ = nullptr;
    }

    // Copy constructor.
    UartByteIO(const UartByteIO& other) = delete;

 private:
    UartByteIO(USART_TypeDef *regs) : regs_(regs) {}

    USART_TypeDef *regs_;
};

}  // namespace io

#endif  // IO_UART_BYTEIO_H_
