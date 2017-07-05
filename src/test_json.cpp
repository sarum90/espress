#include "json.hpp"

#include <mettle.hpp>

#include "test/common.hpp"

#include "jsvalue.hpp"
#include "writer.hpp"

using namespace espress;
using namespace mettle;

struct buffer : public writer {
  int write(std::string_view s) final override {
    data += s;
    return s.size();
  }

  std::string data;
};

test_suite<> json_test_suite("test suite for json encoding", [](auto &_) {
  _.test("number", []() {
    buffer b;
    to_json(jsvalue::number(12.345678), &b);
    expect(b.data, equal_to("12.345678"));
  });

  _.test("bool", []() {
    for (auto & [ in, out ] :
         {std::make_pair(true, "true"), std::make_pair(false, "false")}) {
      buffer b;
      to_json(jsvalue::boolean(in), &b);
      expect(b.data, equal_to(out));
    }
  });

  _.test("simple_string", []() {
    buffer b;
    to_json(jsvalue::string("cat"), &b);
    expect(b.data, equal_to("\"cat\""));
  });

  _.test("string", []() {
    buffer b;
    to_json(jsvalue::string("with\"quot\\es"), &b);
    expect(b.data, equal_to("\"with\\\"quot\\\\es\""));
  });

  _.test("string starting with escaped", []() {
    buffer b;
    to_json(jsvalue::string("\\starting"), &b);
    expect(b.data, equal_to("\"\\\\starting\""));
  });

  _.test("TODO: remove this", []() {
    buffer b;
    expect([&]() { to_json(jsvalue::array(nullptr), &b); },
           thrown<std::runtime_error>());
  });
});
