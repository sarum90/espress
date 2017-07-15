#pragma once

namespace espress {
namespace util {

inline std::string to_string(double d) {
  int64_t i = d;
  if (((double)i) == d) {
    return std::to_string(i);
  }
  constexpr auto buffer_size = 128;
  char buffer[buffer_size];
  size_t s = snprintf(buffer, buffer_size, "%g", d);
  return std::string(buffer, s);
}

} // namespace util
} // namespace espress
