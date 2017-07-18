#pragma once

#include "operators/binary_operator.hpp"

#include "eval_context.hpp"
#include "jsvalue.hpp"
#include "operators/to_string.hpp"
#include "operators/to_number.hpp"

namespace espress {
namespace operators {

namespace {

template <class T>
struct plus_type_traits {
  static constexpr bool prefers_string = false;
};

template <>
struct plus_type_traits<jsstring> {
  static constexpr bool prefers_string = true;
};

template <>
struct plus_type_traits<jsdate> {
  static constexpr bool prefers_string = true;
};

template <>
struct plus_type_traits<jsarray_view> {
  static constexpr bool prefers_string = true;
};

template <>
struct plus_type_traits<jsobject_view> {
  static constexpr bool prefers_string = true;
};

class plus_visitor {
public:
  plus_visitor(eval_context * c): context_(c){}

  template <class T, class U>
  std::enable_if_t<!plus_type_traits<T>::prefers_string && !plus_type_traits<U>::prefers_string, jsvalue>
  operator()(T l, U r) {
    double a = to_number::evaluate(l, context_);
    double b = to_number::evaluate(r, context_);
    return jsvalue::number(a + b);
  }

  template <class T, class U>
  std::enable_if_t<plus_type_traits<T>::prefers_string || plus_type_traits<U>::prefers_string, jsvalue>
  operator()(T l, U r) {
    // TODO: We should instead pass some sort of short-lived context here.
    jsstring a = to_string::evaluate(l, context_);
    jsstring b = to_string::evaluate(r, context_);
    std::string s{a.string_view()};
    s += b.string_view();
    return jsvalue::string(context_->add_string(std::move(s)));
  }

private:
  eval_context * context_;

};

} // anonymous anonymous

class plus : public binary_operator<plus> {
public:
  inline static jsvalue evaluate(jsvalue l, jsvalue r, eval_context *c) {
    plus_visitor visitor(c);
    return espress::visit(visitor, l, r);
  }
};

}  // namespace operators
}  // namespace espress
