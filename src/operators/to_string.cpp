#include "operators/to_string.hpp"

namespace espress {
namespace operators {

namespace detail {

jsstring to_string_visitor::operator()(jsarray_view a) {
  to_string_visitor_for_array my_visit{this};
  std::string result;
  bool first = true;
  for (auto v : a) {
    if (first) {
      first = false;
    } else {
      result.append(",");
    }
    result.append(v.visit(my_visit).string_view());
  }
  return context_->add_string(std::move(result));
}

} // namespace detail

jsvalue to_string::evaluate(jsvalue v, eval_context *c) {
  detail::to_string_visitor visitor(c);
  return jsvalue::string(v.visit(visitor));
}

}  // namespace operators
}  // namespace espress
