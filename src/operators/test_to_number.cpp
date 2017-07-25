#include "operators/to_number.hpp"

#include "test/common.hpp"
#include "test/jsvalue_tools.hpp"
#include "test/node_runner.hpp"

#include "buffer.hpp"
#include "json.hpp"
#include "util/write.hpp"
#include "writer.hpp"

using namespace espress;
using namespace mettle;

test_suite<> to_number_tests("tests to_number", [](auto &_) {
  _.test("test against node command", []() {
    node_runner nr;
    eval_context bigc;
    for (auto j : test::get_complex_values(&bigc)) {
      buffer b;
      util::write_all(&b, "(");
      to_js(j, &b);
      util::write_all(&b, " - 0)");

      eval_context c;
      buffer b2;
      to_espress_json(operators::to_number::evaluate(j, &c), &b2);
      expect(b2.string(), equal_to(nr.run(b.string())));
    }
  });
});
