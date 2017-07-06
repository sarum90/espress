#include "json.hpp"

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

  _.test("date", []() {
    for (auto[in, out] :
         {std::make_pair(1499280807982, "2017-07-05T18:53:27.982Z"),
          std::make_pair(949381200000, "2000-02-01T05:00:00.000Z")}) {
      auto t = std::chrono::milliseconds(in);
      jsdate d{t};
      buffer b;
      to_json(jsvalue::date(d), &b);
      expect(b.data, equal_to(out));
    }
  });

  _.test("array", []() {
    jsarray arr;
    arr.push_back(jsvalue::number(45));
    arr.push_back(jsvalue::string("cat"));
    arr.push_back(jsvalue::boolean(false));
    buffer b;
    to_json(jsvalue::array(&arr), &b);
    expect(b.data, equal_to("[45,\"cat\",false]"));
  });

  _.test("object", []() {
    jsobject o;
    o.set("cat", jsvalue::number(111));
    o.set("mouse", jsvalue::string("hi"));
    o.set("dog", jsvalue::boolean(true));
    buffer b;
    to_json(jsvalue::object(&o), &b);
    expect(b.data, equal_to("{\"cat\":111,\"dog\":true,\"mouse\":\"hi\"}"));
  });

});
