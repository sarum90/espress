#include "operators/to_number.hpp"

#include <limits>

namespace espress {
namespace operators {

namespace detail {

double to_number_visitor::operator()(jsarray_view a) {
  auto i = a.begin();
  if (i == a.end()) {
    return 0;
  }
  auto v = *i;
  ++i;
  if (i != a.end()) {
    return std::numeric_limits<double>::quiet_NaN();
  }
  to_number_visitor_array visitor(this);
  return v.visit(visitor);
}

}  // namespace detail

jsvalue to_number::evaluate(jsvalue v, eval_context *c) {
  detail::to_number_visitor visitor(c);
  return jsvalue::number(v.visit(visitor));
}

}  // namespace operators
}  // namespace espress
