#include <memory>

#include "external/nanopb/pb_encode.h"

namespace waldo {

void Main() {
    auto a = std::make_unique<void*>(reinterpret_cast<void*>(&pb_encode));
    while (1);
}

}  // namespace waldo

extern "C" int main(void) {
    waldo::Main();
    return 0;
}
