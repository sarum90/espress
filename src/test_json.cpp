#include "json.hpp"

#include <limits>

#include "test/common.hpp"
#include "test/jsvalue_tools.hpp"
#include "test/node_runner.hpp"

#include "buffer.hpp"
#include "eval_context.hpp"
#include "jsvalue.hpp"
#include "writer.hpp"

using namespace espress;
using namespace mettle;

test_suite<> json_test_suite("test suite for json encoding", [](auto &_) {
  _.test("null", []() {
    buffer b;
    to_json(jsvalue::null(), &b);
    expect(b.string(), equal_to("null"));
  });

  _.test("undefined", []() {
    buffer b;
    to_json(jsvalue::undefined(), &b);
    expect(b.string(), equal_to("null"));
  });

  _.test("number", []() {
    buffer b;
    // TODO: enhance this testing.
    to_json(jsvalue::number(12.3456), &b);
    expect(b.string(), equal_to("12.3456"));
  });

  _.test("NaN", []() {
    buffer b;
    to_json(jsvalue::number(std::numeric_limits<double>::quiet_NaN()), &b);
    expect(b.string(), equal_to("null"));
  });

  _.test("Infinity", []() {
    buffer b;
    to_json(jsvalue::number(std::numeric_limits<double>::infinity()), &b);
    expect(b.string(), equal_to("null"));
  });

  _.test("neg Infinity", []() {
    buffer b;
    to_json(jsvalue::number(-std::numeric_limits<double>::infinity()), &b);
    expect(b.string(), equal_to("null"));
  });

  _.test("bool", []() {
    for (auto & [ in, out ] :
         {std::make_pair(true, "true"), std::make_pair(false, "false")}) {
      buffer b;
      to_json(jsvalue::boolean(in), &b);
      expect(b.string(), equal_to(out));
    }
  });

  _.test("simple_string", []() {
    buffer b;
    to_json(jsvalue::string("cat"), &b);
    expect(b.string(), equal_to("\"cat\""));
  });

  _.test("tiny string", []() {
    buffer b;
    to_json(jsvalue::string("0"), &b);
    expect(b.string(), equal_to("\"0\""));
  });

  _.test("string", []() {
    buffer b;
    to_json(jsvalue::string("with\"quot\\es"), &b);
    expect(b.string(), equal_to("\"with\\\"quot\\\\es\""));
  });

  _.test("string starting with escaped", []() {
    buffer b;
    to_json(jsvalue::string("\\starting"), &b);
    expect(b.string(), equal_to("\"\\\\starting\""));
  });

  _.test("date", []() {
    for (auto[in, out] :
         {std::make_pair(1499280807982, "\"2017-07-05T18:53:27.982Z\""),
          std::make_pair(949381200000, "\"2000-02-01T05:00:00.000Z\"")}) {
      auto t = std::chrono::milliseconds(in);
      jsdate d{t};
      buffer b;
      to_json(jsvalue::date(d), &b);
      expect(b.string(), equal_to(out));
    }
  });

  _.test("array", []() {
    jsarray arr;
    arr.push_back(jsvalue::number(45));
    arr.push_back(jsvalue::string("cat"));
    arr.push_back(jsvalue::boolean(false));
    buffer b;
    to_json(jsvalue::array(&arr), &b);
    expect(b.string(), equal_to("[45,\"cat\",false]"));
  });

  _.test("object", []() {
    jsobject o;
    o.set("cat", jsvalue::number(111));
    o.set("mouse", jsvalue::string("hi"));
    o.set("dog", jsvalue::boolean(true));
    buffer b;
    to_json(jsvalue::object(&o), &b);
    expect(b.string(), equal_to("{\"cat\":111,\"dog\":true,\"mouse\":\"hi\"}"));
  });

  _.test("nested", []() {
    jsarray a1;
    a1.push_back(jsvalue::date(std::chrono::milliseconds(1500000000000)));
    a1.push_back(jsvalue::boolean(true));
    a1.push_back(jsvalue::string("\\mouse\""));
    a1.push_back(jsvalue::number(1e12));
    a1.push_back(jsvalue::undefined());
    a1.push_back(jsvalue::null());
    jsobject o1;
    o1.set("arr", jsvalue::array(&a1));
    o1.set("num", jsvalue::number(-12.125));
    o1.set("bool", jsvalue::boolean(true));
    jsarray a2;
    a2.push_back(jsvalue::string("Hello World!"));
    jsarray a3;
    a3.push_back(jsvalue::object(&o1));
    a3.push_back(jsvalue::array(&a2));
    jsobject o2;
    o2.set("cat", jsvalue::string("mouse"));
    o2.set("abc123e", jsvalue::number(-12345));
    jsobject o3;
    o3.set("a", jsvalue::array(&a3));
    o3.set("b", jsvalue::object(&o2));
    o3.set("_cde_", jsvalue::boolean(false));

    buffer b;
    to_json(jsvalue::object(&o3), &b);
    expect(b.string(),
           equal_to(
               "{\"_cde_\":false,"
               "\"a\":["
               "{\"arr\":[\"2017-07-14T02:40:00.000Z\",true,\"\\\\mouse\\\"\","
               "1000000000000,null,null],"
               "\"bool\":true,\"num\":-12.125},"
               "[\"Hello World!\"]],"
               "\"b\":{\"abc123e\":-12345,\"cat\":\"mouse\"}}"));
  });

  _.test("tojs", []() {
    buffer b;
    jsarray a;
    a.push_back(jsvalue::boolean(true));
    a.push_back(jsvalue::boolean(false));
    a.push_back(jsvalue::undefined());
    a.push_back(jsvalue::null());
    jsobject o;
    o.set("array", jsvalue::array(&a));
    o.set("number", jsvalue::number(123));
    o.set("date", jsvalue::date(std::chrono::milliseconds{1500000000000}));
    o.set("string", jsvalue::string("cat"));
    to_js(jsvalue::object(&o), &b);
    expect(b.string(),
           equal_to("{\"array\":[true,false,undefined,null],\"date\":new "
                    "Date(1500000000000),\"number\":123,\"string\":\"cat\"}"));
  });

  _.test("tojs NaN", []() {
    buffer b;
    to_js(jsvalue::number(std::numeric_limits<double>::quiet_NaN()), &b);
    expect(b.string(), equal_to("NaN"));
  });

  _.test("tojs Infinity", []() {
    buffer b;
    to_js(jsvalue::number(std::numeric_limits<double>::infinity()), &b);
    expect(b.string(), equal_to("Infinity"));
  });

  _.test("tojs neg Infinity", []() {
    buffer b;
    to_js(jsvalue::number(-std::numeric_limits<double>::infinity()), &b);
    expect(b.string(), equal_to("-Infinity"));
  });

  _.test("test espress_json", []() {
    node_runner nr;
    eval_context bigc;
    for (auto j : test::get_complex_values(&bigc)) {
      buffer b;
      util::write_all(&b, "(");
      to_js(j, &b);
      util::write_all(&b, ")");

      buffer b2;
      to_espress_json(j, &b2);
      expect(b2.string(), equal_to(nr.run(b.string())));
    }
  });
});
