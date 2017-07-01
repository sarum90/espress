#pragma once

#include "../writer.hpp"

namespace espress {
namespace util {

// TODO(correct error type).
inline void write_all(writer* w, std::string_view s) {
  int i = 0;
  while (i < s.size()) {
    int written = w->write(s.substr(i));
    if (written < 1) {
      throw std::runtime_error("Failed to write even a single byte.");
    }
    i += written;
  }
}

} // namespace util
} // namespace espress

