#pragma once

#include "jsvalue.hpp"

#include "eval_context.hpp"

namespace espress {
namespace test {

std::vector<jsvalue> get_simple_values() {
  static jsarray empty_array;
  static jsobject empty_object;
  return {
      // Undefined values
      jsvalue::undefined(),

      // Null values
      jsvalue::null(),

      // Numeric values
      jsvalue::number(-std::numeric_limits<double>::quiet_NaN()),
      jsvalue::number(-std::numeric_limits<double>::infinity()),
      jsvalue::number(-1e12),
      jsvalue::number(-1),
      jsvalue::number(-1.0/16),
      jsvalue::number(0),
      jsvalue::number(1.0/16),
      jsvalue::number(1),
      jsvalue::number(4.0000000001),
      jsvalue::number(1e12),
      jsvalue::number(std::numeric_limits<double>::infinity()),

      // Boolean values
      jsvalue::boolean(false),
      jsvalue::boolean(true),

      // String values
      jsvalue::string(""),
      jsvalue::string("0"),
      jsvalue::string("NaN"),
      jsvalue::string("Infinity"),
      jsvalue::string("-Infinity"),
      jsvalue::string("-1337"),
      jsvalue::string("3.125"),
      jsvalue::string("   "),
      jsvalue::string("l1337"),
      jsvalue::string("1337l"),
      jsvalue::string(" \t1337 \n"),
      jsvalue::string(" \t1337 1 "),
      jsvalue::string("normal"),
      jsvalue::string("\n\\\"bob"),

      // Date values
      jsvalue::date(std::chrono::milliseconds(1500055835025)),
      jsvalue::date(std::chrono::milliseconds(0)),

      // Empty array
      jsvalue::array(&empty_array),

      // Empty object
      jsvalue::object(&empty_object),
  };
}

std::vector<jsvalue> get_complex_values(eval_context* c) {
  std::vector<jsvalue> objs;
  auto vals = get_simple_values();

  for (auto v : vals) {
    objs.push_back(v);
  }

  for (auto v : vals) {
    auto& array = c->add_array();
    array.push_back(v);
    objs.push_back(jsvalue::array(&array));
  }

  for (size_t i = 0; i+1 < vals.size(); i += 2) {
    auto& array = c->add_array();
    array.push_back(vals[i]);
    array.push_back(vals[i + 1]);
    if (i+3 == vals.size()) {
      array.push_back(vals[i + 2]); // COVERAGE_MISS_OK
    }
    objs.push_back(jsvalue::array(&array));
  }

  for (size_t i = 0; i < vals.size(); i += 2) {
    auto& obj = c->add_object();
    obj.set(c->add_string("key_" + std::to_string(i % 5)), vals[i]);
    if (i + 1 < vals.size()) {
      obj.set(c->add_string("key_" + std::to_string((i + 1) % 5)), vals[i + 1]);
    }
    objs.push_back(jsvalue::object(&obj));
  }

  return objs;
}

}  // namespace test
}  // namespace espress
