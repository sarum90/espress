#pragma once

#include "checked_syscalls.hpp"
#include "reader.hpp"
#include "util/assert.hpp"
#include "writer.hpp"

namespace espress {

class pipe {
public:
  pipe();
  ~pipe();

  pipe(const pipe& other) = delete;
  pipe(pipe&& other);

  pipe& operator=(const pipe& other) = delete;
  pipe& operator=(pipe&& other);

  file_writer writer() {
    util::eassert(write_fd_ > 0, "write fd invalid");
    return file_writer(write_fd_);
  }

  int dup_write() {
    util::eassert(write_fd_ > 0, "write fd invalid");
    return checked_syscalls::dup(write_fd_);
  }

  int dup2_write(int newfd) {
    util::eassert(write_fd_ > 0, "write fd invalid");
    return checked_syscalls::dup2(write_fd_, newfd);
  }

  file_reader reader() {
    util::eassert(read_fd_ > 0, "read fd invalid");
    return file_reader(read_fd_);
  }

  int dup_read() {
    util::eassert(read_fd_ > 0, "read fd invalid");
    return checked_syscalls::dup(read_fd_);
  }

  int dup2_read(int newfd) {
    util::eassert(read_fd_ > 0, "read fd invalid");
    return checked_syscalls::dup2(read_fd_, newfd);
  }

  void reset();

private:
  int read_fd_ = -1;
  int write_fd_ = -1;
};

}  // namespace espress
