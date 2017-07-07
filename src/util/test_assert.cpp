#include "util/assert.hpp"

#include <mettle.hpp>

#include "test/common.hpp"

using namespace espress;
using namespace mettle;

test_suite<> util_write_tests("write util tests", [](auto &_) {
  _.test("failed asserts raise exceptions", []() {
    expect([]() { util::eassert(false, "mousey"); },
           thrown<util::assertion_error>(regex_match(".*mousey.*")));
  });
});
