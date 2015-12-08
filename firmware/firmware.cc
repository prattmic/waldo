#include <memory>
#include <vector>

namespace waldo {
    void Main() {
        auto a = std::make_unique<int>(5);
        std::vector<int> v;
        while (1);
    }
}

extern "C" int main(void) {
    waldo::Main();
    return 0;
}
