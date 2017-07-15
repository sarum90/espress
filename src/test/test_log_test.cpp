#include "test/log_test.hpp"

#include "test/common.hpp"

#include "checked_syscalls.hpp"
#include "pipe.hpp"
#include "util/read.hpp"

using namespace espress;
using namespace mettle;

struct myerror {};

template <class T>
class defer {
public:
  defer(T&& f) : f_(std::forward<T>(f)) {}
  ~defer() { f_(); }

private:
  T f_;
};

template <class T>
defer<T> make_defer(T&& t) {
  return defer<T>(std::forward<T>(t));
}

test_suite<> log_tests("tests for log_test", [](auto& _) {
  _.test("prints message on stderr if exception occurs", []() {
    int pipe_read = -1;
    auto close_pipe = make_defer([&]() {
      if (pipe_read >= 0) {
        checked_syscalls::close(pipe_read);
      }
    });

    {
      auto backup_stderr = checked_syscalls::dup(2);
      auto restore = make_defer([&]() {
        checked_syscalls::dup2(backup_stderr, 2);
        checked_syscalls::close(backup_stderr);
      });

      {
        espress::pipe p;
        p.dup2_write(2);
        pipe_read = p.dup_read();
      }

      try {
        log_test lt;
        lt << "hi\n";
        throw myerror{};
      } catch (myerror& e) {
      }
    }

    file_reader r(pipe_read);
    std::string s(3, '\0');
    util::read_all(&r, s);
    expect(s, equal_to("hi\n"));
  });

  _.test("Does not print if there is no exception.", []() {
    int pipe_read = -1;
    auto close_pipe = make_defer([&]() {
      if (pipe_read >= 0) {
        checked_syscalls::close(pipe_read);
      }
    });

    {
      auto backup_stderr = checked_syscalls::dup(2);
      auto restore = make_defer([&]() {
        checked_syscalls::dup2(backup_stderr, 2);
        checked_syscalls::close(backup_stderr);
      });

      {
        espress::pipe p;
        p.dup2_write(2);
        pipe_read = p.dup_read();
      }

      log_test lt;
      lt << "hi\n";
    }

    file_reader r(pipe_read);
    std::string s(3, '\0');
    expect(r.read(s), equal_to(0));  // EOF
  });
});
