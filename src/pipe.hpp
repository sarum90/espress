#pragma once

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
  file_reader reader() {
    util::eassert(read_fd_ > 0, "read fd invalid");
    return file_reader(read_fd_);
  }

private:
  int read_fd_ = -1;
  int write_fd_ = -1;
};

}  // namespace espress
