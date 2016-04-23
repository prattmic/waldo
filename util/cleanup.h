#ifndef UTIL_CLEANUP_H_
#define UTIL_CLEANUP_H_

#include <functional>

namespace util {

class Cleanup {
 public:
     Cleanup(std::function<void()> func) : func_(func) {}

     ~Cleanup() {
         func_();
     }

 private:
     std::function<void()> func_;
};

}  // namepsace util

#endif  // UTIL_CLEANUP_H_
