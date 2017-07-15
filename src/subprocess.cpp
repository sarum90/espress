
#include <unistd.h>
#include <iostream>

#include "checked_syscalls.hpp"
#include "coverage.hpp"
#include "subprocess.hpp"

namespace espress {

subprocess::subprocess(int stdin_fd, int stdout_fd, pid_t child_pid)
    : std_in_(stdin_fd),
      std_out_(stdout_fd),
      child_pid_(child_pid),
      stdin_closer_(stdin_fd),
      stdout_closer_(stdout_fd) {}

subprocess::~subprocess() noexcept(false) {
  int res = 0;
  stdin_closer_.close();
  checked_syscalls::waitpid(child_pid_, &res, 0);
  if (std::uncaught_exception()) {
    return;
  }
  // TODO: replace these assertions with custom exceptions.
  util::eassert(WIFEXITED(res),
                "subprocess did not exit normally (killed, coredumped, ...)");
  util::eassert(WEXITSTATUS(res) == 0,
                "subprocess exited with non-zero status.");
}

subprocess subprocess::create(std::vector<std::string> args, std::map<std::string, std::string> env) {
  pipe std_in;
  pipe std_out;
  pid_t child = checked_syscalls::fork();
  if (child == 0) {
    // in child.
    std_out.dup2_write(STDOUT_FILENO);
    std_in.dup2_read(STDIN_FILENO);

    // Clean up remaining fds from the pipes:
    std_out.reset();
    std_in.reset();

    std::vector<char *> argv;
    for (auto &s : args) {
      argv.push_back(&s[0]);
    }
    argv.push_back(nullptr);

    std::vector<char *> envp;
    std::vector<std::string> envp_strings;
    for (auto [k, v] : env) {
      std::string s = k + "=" + v;
      envp_strings.push_back(s);
      envp.push_back(&envp_strings[envp_strings.size()-1][0]);
    }
    envp.push_back(nullptr);
    file_writer w(STDOUT_FILENO);
    try {
      coverage_before_exit();
      checked_syscalls::execvpe(argv[0], &argv[0], &envp[0]);
    } catch (...) {
      coverage_before_exit();
      std::terminate();  // COVERAGE_MISS_OK
    }
  }
  return subprocess(std_in.dup_write(), std_out.dup_read(), child);
}

}  // namespace espress
