#include "reader.hpp"

#include "test/common.hpp"

using namespace espress;
using namespace mettle;

struct bad_reader {
  static inline ssize_t read(int fd, const void *buf, size_t count) {
    errno = EIO;
    return -1;
  }
};

test_suite<> failed_reads_test("file_reader failed reads.", [](auto &_) {
  _.test("EIO test", []() {
    file_reader_impl<bad_reader> fr(0);
    std::string buff(100, '\0');
    expect([&]() { fr.read(buff); }, thrown<std::runtime_error>());
  });
});
