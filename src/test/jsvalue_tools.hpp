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
    jsvalue::number(-1e12),
    jsvalue::number(-1),
    jsvalue::number(-0.001),
    jsvalue::number(0),
    jsvalue::number(0.001),
    jsvalue::number(1),
    jsvalue::number(1e12),

    // Boolean values
    jsvalue::boolean(false),
    jsvalue::boolean(true),

    // String values
    jsvalue::string(""),
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

std::vector<jsvalue> get_complex_values(eval_context *c) {
  std::vector<jsvalue> objs;
  auto vals = get_simple_values();

  for (size_t i = 0; i < vals.size(); i+= 2) {
    auto& array = c->add_array();
    array.push_back(vals[i]);
    if (i+1 < vals.size()) {
      array.push_back(vals[i+1]);
    }
    objs.push_back(jsvalue::array(&array));
  }

  for (size_t i = 0; i < vals.size(); i+= 2) {
    auto& obj = c->add_object();
    obj.set(c->add_string("key_" + std::to_string(i % 5)), vals[i]);
    if (i+1 < vals.size()) {
      obj.set(c->add_string("key_" + std::to_string((i+1) % 5)), vals[i+1]);
    }
    objs.push_back(jsvalue::object(&obj));
  }

  for (auto v: vals) {
    objs.push_back(v);
  }

  return objs;
}

}  // namespace test
}  // namespace espress
