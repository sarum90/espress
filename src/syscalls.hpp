#pragma once

#include <unistd.h>

namespace espress {

// Used for dependency injection to test errors that can come from syscalls.
struct syscalls {
  static inline ssize_t write(int fd, const void *buf, size_t count) {
    return ::write(fd, buf, count);
  }

  static inline ssize_t read(int fd, void *buf, size_t count) {
    return ::read(fd, buf, count);
  }

  static inline int pipe(int pipefd[2]) { return ::pipe(pipefd); }

  static inline int close(int fd) { return ::close(fd); }
};

}  // namespace espress
