#include <streambuf>
#include <memory>
#include <utility>
#include "io/byteio.h"

namespace io {

class ByteIOStreamBuf : public std::streambuf {
 public:
    ByteIOStreamBuf(std::unique_ptr<ByteIO> io) : io_(std::move(io)) {}

 protected:
    virtual std::streambuf::int_type overflow(std::streambuf::int_type c) override {
        if (c == std::streambuf::traits_type::eof())
            return std::streambuf::traits_type::eof();

        auto status = io_->Write(c);
        if (!status.ok())
            return std::streambuf::traits_type::eof();

        return 1;
    }

 private:
    std::unique_ptr<ByteIO> io_;
};

}  // namespace io
