#include "writer.hpp"

#include "test/common.hpp"

using namespace espress;
using namespace mettle;

struct bad_writer {
  static inline ssize_t write(int fd, const void *buf, size_t count) {
    errno = EBADF;
    return -1;
  }
};

test_suite<> failed_writes_test("file_writer tests for failed writes.", [](auto &_) {
  _.test("EBADF test", [](){
    file_writer_impl<bad_writer> fw(0);
    expect([&](){fw.write("cat");}, thrown<std::runtime_error>());
  });
});
