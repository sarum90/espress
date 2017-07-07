#pragma once

// C includes:
#include <errno.h>
#include <stdio.h>

// C++ includes:
#include <stdexcept>
#include <string_view>

#include "syscalls.hpp"

namespace espress {

class writer {
public:
  virtual int write(std::string_view s) = 0;
};

template <typename syscalls>
class file_writer_impl : public writer {
public:
  explicit file_writer_impl(int fd) : fd_(fd) {}

  int write(std::string_view s) final override {
    int r = syscalls::write(fd_, &s[0], s.size());
    // TODO: fix exception type, handle EAGAIN gracefully, test.
    if (r == -1) {
      perror("Error while writing to a fd:");
      throw std::runtime_error("Error on write, see stderr.");
    }
    return r;
  }

private:
  const int fd_;
};

typedef file_writer_impl<syscalls> file_writer;

}  // namespace espress
