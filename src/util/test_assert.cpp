#include "util/assert.hpp"

#include <mettle.hpp>

#include "test/common.hpp"

using namespace espress;
using namespace mettle;

test_suite<> util_write_tests("write util tests", [](auto &_) {
  _.test("raises error on bad write", []() {
    expect([]() { util::eassert(false, "mousey"); },
           thrown<std::runtime_error>(regex_match(".*mousey.*")));
  });
});
