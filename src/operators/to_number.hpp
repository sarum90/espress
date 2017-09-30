#pragma once

#include "eval_context.hpp"
#include "operators/unary_operator.hpp"

namespace espress {
namespace operators {

namespace detail {

class to_number_visitor {
public:
  to_number_visitor(eval_context *c) : context_(c) {}

  double operator()(jsundefined u) {
    return std::numeric_limits<double>::quiet_NaN();
  }
  double operator()(jsnull u) { return 0; }
  double operator()(bool b) { return b ? 1 : 0; }
  double operator()(double d) { return d; }
  double operator()(jsstring s) {
    if (s.size() == 0) {
      return 0;
    }
    std::string str{
        s.string_view()};  // A copy just to add a null terminator :(.
    const char *c = str.c_str();
    char *e = nullptr;
    double d = strtod(c, &e);

    while (std::isspace(*e)) {
      ++e;
    }

    if (e != c + str.size()) {
      return std::numeric_limits<double>::quiet_NaN();
    }
    return d;
  }
  double operator()(jsdate d) { return d.time_since_epoch().count(); }

  double operator()(jsarray_view a);

  double operator()(jsobject_view o) {
    return std::numeric_limits<double>::quiet_NaN();
  }

private:
  eval_context *context_;
};

// For arrays, the conversion is actually array -> string -> number. This
// visitor is only used for arrays with exactly 1 element.
//
// This saves us a value->string conversion, since we know that for most types
// that will result in either an invalid number or a zero length string.
class to_number_visitor_array {
public:
  to_number_visitor_array(to_number_visitor *v) : visitor_(v) {}

  inline double operator()(jsundefined u) { return 0; }
  inline double operator()(jsnull u) { return 0; }
  inline double operator()(bool b) {
    return std::numeric_limits<double>::quiet_NaN();
  }
  inline double operator()(double d) { return (*visitor_)(d); }
  inline double operator()(jsstring s) { return (*visitor_)(s); }
  inline double operator()(jsdate d) {
    return std::numeric_limits<double>::quiet_NaN();
  }
  inline double operator()(jsarray_view a) { return (*visitor_)(a); }
  inline double operator()(jsobject_view o) { return (*visitor_)(o); }

private:
  to_number_visitor *visitor_;
};

}  // namespace detail

class to_number : public unary_operator<to_number, double> {
public:
  typedef detail::to_number_visitor visitor;
};

}  // namespace operators
}  // namespace espress
