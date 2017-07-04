#pragma once

#include <stdexcept>
#include <string_view>

namespace espress {
namespace util {

inline void eassert(bool must_be_true, std::string_view message) {
  if (!must_be_true) {
    std::string s = "Assertion failure: ";
    s += message;
    throw std::runtime_error(s);
  }
}

} // namespace util
} // namespace espress

