#include "checked_syscalls.hpp"

#include "test/common.hpp"

using namespace espress;
using namespace mettle;

struct pipe_error_syscalls : public syscalls {
  static inline int pipe(int pipefds[2]) {
    errno = EMFILE;
    return -1;
  }
};

test_suite<> checked_syscalls_tests("tests for checked_syscalls", [](auto &_) {
    int fds[2];
    expect([&](){ checked_syscalls_impl<pipe_error_syscalls>::pipe(fds); }, thrown<std::system_error>("Too many open files"));
});
