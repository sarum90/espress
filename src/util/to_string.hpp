#pragma once

#include <cmath>
#include <limits>

#include "util/assert.hpp"

namespace espress {
namespace util {

inline std::string to_string(double d) {
  if (std::isnan(d)) {
    return "NaN";
  } else if (std::isinf(d)) {
    if (d < 0) {
      return "-Infinity";
    } else {
      return "Infinity";
    }
  }
  int64_t i = d;
  if (((double)i) == d) {
    return std::to_string(i);
  }
  constexpr auto buffer_size = 128;
  char buffer[buffer_size];
  size_t s = snprintf(buffer, buffer_size, "%f", d);
  size_t new_s = s;
  while (new_s > 0 && buffer[new_s - 1] == '0') {
    new_s--;
  }
  for (size_t i = 1; i < new_s; i++) {
    if (buffer[i] == '.') {
      if (i + 1 == new_s) {
        return std::string(buffer, new_s - 1);
      }
      return std::string(buffer, new_s);
    }
  }
  util::eassert(  // COVERAGE_MISS_OK
      false,
      "printf %f produced something without a '.'");  // COVERAGE_MISS_OK
  return std::string{};                               // COVERAGE_MISS_OK
}

}  // namespace util
}  // namespace espress
