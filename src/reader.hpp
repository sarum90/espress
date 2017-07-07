#pragma once

#include "syscalls.hpp"
#include "util/assert.hpp"

namespace espress {

class read_buffer {
public:
  read_buffer() {}

  read_buffer(char* c, size_t size) : c_(c), size_(size) {}

  // only valid until s is modified.
  read_buffer(std::string& s) : c_(&s[0]), size_(s.size()) {}

  inline read_buffer substr(size_t pos) {
    util::eassert(pos <= size_, "substr off end of read_buffer.");
    return read_buffer{c_ + pos, size_ - pos};
  }

  char* data() { return c_; }
  size_t size() { return size_; }

private:
  char* c_ = nullptr;
  size_t size_ = 0;
};

class reader {
public:
  virtual int read(read_buffer s) = 0;
};

template <typename syscalls>
class file_reader_impl : public reader {
public:
  explicit file_reader_impl(int fd) : fd_(fd) {}

  int read(read_buffer s) final override {
    int r = syscalls::read(fd_, s.data(), s.size());
    // TODO: fix exception type, handle EAGAIN gracefully, test.
    if (r == -1) {
      perror("Error while reading from a fd:");
      throw std::runtime_error("Error on read, see stderr.");
    }
    return r;
  }

private:
  const int fd_;
};

typedef file_reader_impl<espress::syscalls> file_reader;

}  // namespace espress
