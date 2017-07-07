#pragma once

#include <system_error>

#include "syscalls.hpp"

namespace espress {

template <typename syscalls>
struct checked_syscalls_impl {
public:
  static inline void close(int fd) {
    check_retval(syscalls::close(fd));
  }

  static inline void pipe(int fd[2]) {
    check_retval(syscalls::pipe(fd));
  }

private:
  static inline void check_retval(int r) {
    if (r < 0) {
      throw std::system_error(errno, std::system_category());
    }
  }
};

typedef checked_syscalls_impl<espress::syscalls> checked_syscalls;

} // namespace espress
