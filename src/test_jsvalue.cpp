#include "jsvalue.hpp"

#include "test/common.hpp"
#include "test/jsvalue_tools.hpp"
#include "test/log_test.hpp"

#include "eval_context.hpp"
#include "util/variant.hpp"

#include <type_traits>
#include "json.hpp"

using namespace espress;
using namespace mettle;

test_suite<> jsvalue_jsdatae("jsdate tests", [](auto &_) {
  _.test("date", []() {
    constexpr auto t = std::chrono::milliseconds(1499280807982);
    jsdate d{t};
    expect(
        d.to_time_t(),
        equal_to(std::chrono::duration_cast<std::chrono::seconds>(t).count()));
  });

  _.test("default time_point is 1970", []() {
    tm epoch = {0};
    epoch.tm_mday = 1;
    epoch.tm_year = 70;
    time_t t = timegm(&epoch);
    expect(t, equal_to(0));
    auto tp = std::chrono::system_clock::from_time_t(t);
    expect(std::chrono::system_clock::time_point{}, equal_to(tp));
  });

  _.test("equality works as intended", []() {
    eval_context c;
    auto vals = test::get_complex_values(&c);
    for (size_t i = 0; i < vals.size(); i++) {
      for (size_t j = 0; j < vals.size(); j++) {
        log_test lt;
        lt << i << "," << j << std::endl;
        expect(vals[i] == vals[j], equal_to(i == j));
      }
    }
  });
});
