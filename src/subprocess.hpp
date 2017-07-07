#pragma once

#include <vector>

#include "pipe.hpp"
#include "reader.hpp"
#include "util/closer.hpp"
#include "writer.hpp"

namespace espress {

class subprocess {
public:
  static subprocess create(std::vector<std::string> args);

  // waits for subprocess.
  ~subprocess() noexcept(false);

  // writer to write to stdin.
  writer* std_in() {return &std_in_;}

  // reader to read from stdout.
  reader* std_out() {return &std_out_;}

private:
  subprocess(int stdin_fd, int stdout_fd, pid_t child_pid);

  file_writer std_in_;
  file_reader std_out_;
  const pid_t child_pid_;

  // Members to close the stdin / stdout pipes on destruction.
  util::closer stdin_closer_;
  util::closer stdout_closer_;
};

}  // namespace espress
