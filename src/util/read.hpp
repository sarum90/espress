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
      throw std::runtime_error("Unexpected EOF.");
    }
    i += readed;
  }
}

// TODO(correct error type).
// Reads until an invocation of read returns a string that contains 'c', or the
// buffer is filled up.
//
// N.B. it might return beyond that character, as there is no way to push bytes
// back into a file descriptor, and I don't want to read things 1 byte at a
// time or add a buffer layer at this point.
//
// TODO: make a buffered reader, advocate using that instead.
inline size_t read_to(reader *r, char c, read_buffer s) {
  size_t i = 0;
  size_t ii = 0;
  const char *buff = s.data();
  while (i < s.size()) {
    int readed = r->read(s.substr(i));
    if (readed < 1) {
      throw std::runtime_error("Unexpected EOF.");
    }
    i += readed;
    for (; ii < i; ii++) {
      if (buff[ii] == c) {
        return i;
      }
    }
  }
  return i;
}

}  // namespace util
}  // namespace espress
