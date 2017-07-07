#pragma once

#include <stdexcept>
#include <string_view>

namespace espress {
namespace util {

class assertion_error : public std::logic_error {
public:
  assertion_error(const std::string& what_arg) : std::logic_error(what_arg) {}
  assertion_error(const char* what_arg) : std::logic_error(what_arg) {}
};

inline void eassert(bool must_be_true, std::string_view message) {
  if (!must_be_true) {
    std::string s = "Assertion failure: ";
    s += message;
    throw assertion_error(s);
  }
}

}  // namespace util
}  // namespace espress
