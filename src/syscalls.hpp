#pragma once

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

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

  static inline pid_t fork() { return ::fork(); }

  static inline int dup(int fd) { return ::dup(fd); }

  static inline int dup2(int fd, int newfd) { return ::dup2(fd, newfd); }

  static inline pid_t waitpid(pid_t pid, int *status, int options) {
    return ::waitpid(pid, status, options);
  }

  static inline int execvpe(const char *file, char *const argv[], char *const envp[]) {
    return ::execvpe(file, argv, envp);
  }
};

}  // namespace espress
