#include "util/read.hpp"

#include "test/common.hpp"

using namespace espress;
using namespace mettle;

struct bad_reader : public reader {
  int read(read_buffer r) final override { return 0; }
};

test_suite<> util_read_tests("read util tests", [](auto &_) {
  _.test("raises error on bad read", []() {
    bad_reader bad;
    std::string s(100, '\0');
    expect([&]() { util::read_all(&bad, s); },
           thrown<std::runtime_error>());
  });
});
