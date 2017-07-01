#pragma once

// C includes:
#include <errno.h>
#include <stdio.h>
#include <unistd.h>

// C++ includes:
#include <stdexcept>
#include <string_view>

namespace espress {

class writer {
public:
  virtual int write(std::string_view s) = 0;
};

class file_writer: public writer {
public:
  file_writer(int fd): fd_(fd) {}

  int write(std::string_view s) final override {
    int r = ::write(fd_, &s[0], s.size());
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


} // namespace espress
