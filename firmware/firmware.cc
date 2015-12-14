#include <memory>

#include "external/nanopb/pb_encode.h"
#include "firmware/simple.pb.hpp"

namespace waldo {

void Main() {
    auto a = std::make_unique<void*>(nullptr);
    Simple s;
    s.set_double_field(5.0);
    uint8_t buf[Simple_size];
    s.Serialize(buf, Simple_size);
    s.Deserialize(buf, Simple_size);
}

}  // namespace waldo

extern "C" int main(void) {
    waldo::Main();
    return 0;
}
