#pragma once

#include "checked_syscalls.hpp"

namespace espress {
namespace util {

class closer {
  public:
  closer(int fd): fd_(fd) {}
  ~closer() { close(); }

  inline void close() {
    if (fd_ > 0) checked_syscalls::close(fd_);
    fd_ = -1;
  }

private:
  int fd_;
};

} // namespace util
} // namespace espress
