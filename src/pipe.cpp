#include "pipe.hpp"

#include "checked_syscalls.hpp"

namespace espress {

pipe::pipe() {
  int fds[2];
  checked_syscalls::pipe(fds);
  read_fd_ = fds[0];
  write_fd_ = fds[1];
}

pipe::~pipe() {
  if (read_fd_ > 0) {
    checked_syscalls::close(read_fd_);
  }
  if (write_fd_ > 0) {
    checked_syscalls::close(write_fd_);
  }
}

pipe::pipe(pipe&& other) {
  read_fd_ = other.read_fd_;
  other.read_fd_ = -1;
  write_fd_ = other.write_fd_;
  other.write_fd_ = -1;
}

pipe& pipe::operator=(pipe&& other) {
  if (read_fd_ > 0) {
    checked_syscalls::close(read_fd_);
  }
  read_fd_ = other.read_fd_;
  other.read_fd_ = -1;

  if (write_fd_ > 0) {
    checked_syscalls::close(write_fd_);
  }
  write_fd_ = other.write_fd_;
  other.write_fd_ = -1;
  return (*this);
}

void pipe::reset() {
  pipe unused(std::move(*this));
}

}
