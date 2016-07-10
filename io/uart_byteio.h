#ifndef IO_UART_BYTEIO_H_
#define IO_UART_BYTEIO_H_

#include <unistd.h>
#include "external/com_github_prattmic_nanopb/util/task/status.h"
#include "external/com_github_prattmic_nanopb/util/task/statusor.h"
#include "io/byteio.h"
#include "third_party/gecko_sdk/Device/SiliconLabs/EFM32HG/Include/em_device.h"

namespace io {

class RingBuffer {
 public:
    util::StatusOr<char> Read() {
        __disable_irq();
        if (start_ != end_) {
            char c = buf_[start_];
            start_ = (start_+1) % kBufferSize;
            __enable_irq();
            return c;
        }
        __enable_irq();
        return util::Status(util::error::Code::RESOURCE_EXHAUSTED,
                            "buffer empty");
    }

    // Only to be called by interrupt handler.
    void Write(char c) {
        buf_[end_] = c;
        end_ = (end_+1) % kBufferSize;
    }

 private:
    static constexpr int kBufferSize = 512;

    char buf_[kBufferSize];
    int start_ = 0;
    int end_ = 0;
};

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
    UartByteIO(UartByteIO&& other)
        : regs_(other.regs_), rx_buf_(other.rx_buf_) {
        other.regs_ = nullptr;
        other.rx_buf_ = nullptr;
    }

    // Copy constructor.
    UartByteIO(const UartByteIO& other) = delete;

 private:
    UartByteIO(USART_TypeDef *regs, RingBuffer* rx_buf)
        : regs_(regs), rx_buf_(rx_buf) {}

    USART_TypeDef* regs_;
    RingBuffer* rx_buf_;
};

}  // namespace io

#endif  // IO_UART_BYTEIO_H_
