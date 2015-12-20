#ifndef SIM808_SIM808_H_
#define SIM808_SIM808_H_

#include <memory>
#include <utility>
#include "external/nanopb/util/task/status.h"
#include "external/nanopb/util/task/statusor.h"
#include "io/byteio.h"

namespace sim808 {

using ::util::Status;

class SIM808 {
 public:
    SIM808(std::unique_ptr<io::ByteIO> io) : io_(std::move(io)) {}

    Status Initialize();

 private:
    std::unique_ptr<io::ByteIO> io_;
};

}  // namespace sim808

#endif  // SIM808_SIM808_H_
