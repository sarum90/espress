#pragma once

#include <system_error>

#include "syscalls.hpp"

namespace espress {

template <typename syscalls>
struct checked_syscalls_impl {
public:
  static inline void close(int fd) { check_retval(syscalls::close(fd)); }

  static inline void pipe(int fd[2]) { check_retval(syscalls::pipe(fd)); }

  static inline pid_t fork() { return check_retval(syscalls::fork()); }

  static inline int dup(int fd) { return check_retval(syscalls::dup(fd)); }

  static inline int dup2(int fd, int newfd) {
    return check_retval(syscalls::dup2(fd, newfd));
  }

  static inline pid_t waitpid(pid_t pid, int *status, int options) {
    return check_retval(syscalls::waitpid(pid, status, options));
  }

  static inline int execvpe(const char *file,
                            char *const argv[],
                            char *const envp[]) {
    return check_retval(syscalls::execvpe(file, argv, envp));
  }

private:
  template <class T>
  static inline T check_retval(T r) {
    if (r < 0) {
      throw std::system_error(errno, std::system_category());
    }
    return r;
  }
};

typedef checked_syscalls_impl<espress::syscalls> checked_syscalls;

}  // namespace espress
