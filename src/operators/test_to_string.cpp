#include "operators/to_string.hpp"

#include "test/common.hpp"
#include "test/node_runner.hpp"
#include "test/jsvalue_tools.hpp"

#include "json.hpp"
#include "writer.hpp"
#include "util/write.hpp"

using namespace espress;
using namespace mettle;

class buffer: public writer {
public:
  int write(std::string_view s) override final {
    data_ += s;
    return s.size();
  }

  std::string_view string() {
    return data_;
  }

private:
  std::string data_;
};

test_suite<> to_string_tests("tests to_string", [](auto &_) {
  _.test("bool to string", [] () {
    jsvalue v = jsvalue::undefined();
    eval_context c;
    expect(operators::to_string::evaluate(v, &c), equal_to(jsvalue::string("undefined")));
  });

  _.test("test running a node command", [] () {
      node_runner nr;
      eval_context bigc;
      for (auto j : test::get_complex_values(&bigc)) {
        buffer b;
        to_js(j, &b);
        util::write_all(&b, " + \"\"");

        file_writer fw(2);
        to_js(j, &fw);

        eval_context c;
        buffer b2;
        to_json(operators::to_string::evaluate(j, &c), &b2);
        expect(b2.string(),  equal_to(nr.run(b.string())));
      }
  });
});
