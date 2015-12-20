#include "external/nanopb/util/task/status.h"
#include "external/nanopb/util/task/statusor.h"
#include "sim808/sim808.h"

namespace sim808 {

Status SIM808::Initialize() {
    auto status = io_->Write('A');
    if (!status.ok())
        return status;

    status = io_->Write('T');
    if (!status.ok())
        return status;

    status = io_->Write('\r');
    if (!status.ok())
        return status;

    status = io_->Write('\n');
    if (!status.ok())
        return status;

    return Status::OK;
}

}  // namespace sim808
