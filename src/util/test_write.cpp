#include "util/write.hpp"

#include <mettle.hpp>

#include "test/common.hpp"

using namespace espress;
using namespace mettle;

struct bad_writer : public writer {
  int write(std::string_view s) final override { return 0; }
};

test_suite<> util_write_tests("write util tests", [](auto &_) {
  _.test("raises error on bad write", []() {
    bad_writer bad;
    expect([&]() { util::write_all(&bad, "kittycat"); },
           thrown<std::runtime_error>());
  });
});
