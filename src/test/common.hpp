#pragma once

#include <mettle.hpp>

#include "coverage.hpp"

namespace espress {

template <typename... Fixtures>
class test_suite {
public:
  template <typename... Args>
  test_suite(Args &&... args)
      : suite("Espress test suite", [&](auto &_) {
          _.teardown([]() { coverage_before_exit(); });
          mettle::subsuite<Fixtures...>(_, std::forward<Args>(args)...);
        }) {}

private:
  mettle::suite<> suite;
};

}  // namespace espress
