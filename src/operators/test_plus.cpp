#include "operators/plus.hpp"

#include "test/common.hpp"
#include "test/jsvalue_tools.hpp"
#include "test/node_runner.hpp"

#include "json.hpp"
#include "util/write.hpp"
#include "writer.hpp"

using namespace espress;
using namespace mettle;

class buffer : public writer {
public:
  int write(std::string_view s) override final {
    data_ += s;
    return s.size();
  }

  std::string_view string() { return data_; }

private:
  std::string data_;
};

test_suite<> plus_tests("tests pluss", [](auto &_) {
  _.test("test against node commands", []() {
    node_runner nr;
    eval_context bigc;
    // TODO: this takes a long time, it'd be nice if we could shorten it.
    for (auto j : test::get_complex_values(&bigc)) {
      for (auto j2 : test::get_complex_values(&bigc)) {
        buffer b;
        util::write_all(&b, "(");
        to_js(j, &b);
        util::write_all(&b, ") + (");
        to_js(j2, &b);
        util::write_all(&b, ")");

        file_writer fw(2);
        to_js(j, &fw);
        util::write_all(&fw, "\n");
        to_js(j2, &fw);
        util::write_all(&fw, "\n\n");

        eval_context c;
        buffer b2;
        to_espress_json(operators::plus::evaluate(j, j2, &c), &b2);
        expect(b2.string(), equal_to(nr.run(b.string())));
      }
    }
  });
});
