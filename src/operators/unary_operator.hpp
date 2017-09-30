#pragma once

#include "jsvalue.hpp"

namespace espress {
namespace operators {

struct variable_return_type {};

template <class sub, class return_type = variable_return_type>
class unary_operator {
public:
  constexpr static bool is_unary_operator = true;
  constexpr static bool is_binary_operator = false;

  template <class T>
  static return_type evaluate(T t, eval_context *c) {
    typename sub::visitor v(c);
    return v(t);
  }

  static jsvalue evaluate(jsvalue v, eval_context *c) {
    typename sub::visitor visitor(c);
    return jsvalue::make(v.visit(visitor));
  }
};

template <class sub>
class unary_operator<sub, variable_return_type> {
public:
  constexpr static bool is_unary_operator = true;
  constexpr static bool is_binary_operator = false;

  template <class T>
  static jsvalue evaluate(T &&v, eval_context *c) {
    typename sub::visitor visitor(c);
    return jsvalue::make(v.visit(visitor));
  }
};

}  // namespace operators
}  // namespace espress
