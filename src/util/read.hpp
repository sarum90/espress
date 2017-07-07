#pragma once

#include "reader.hpp"

namespace espress {
namespace util {

// TODO(correct error type).
inline void read_all(reader *r, read_buffer s) {
  size_t i = 0;
  while (i < s.size()) {
    int readed = r->read(s.substr(i));
    if (readed < 1) {
      throw std::runtime_error("Failed to read even a single byte.");
    }
    i += readed;
  }
}

}  // namespace util
}  // namespace espress
