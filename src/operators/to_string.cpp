#include "operators/to_string.hpp"

#include "util/to_string.hpp"

namespace espress {
namespace operators {

namespace {

class to_string_visitor {
public:
  to_string_visitor(eval_context *c) : context_(c) {}

  jsstring operator()(jsundefined u) { return "undefined"; }
  jsstring operator()(jsnull u) { return "null"; }
  jsstring operator()(bool b) {
    if (b) return "true";
    return "false";
  }
  jsstring operator()(double d) {
    return context_->add_string(util::to_string(d));
  }
  jsstring operator()(jsstring s) { return s; }
  jsstring operator()(jsdate d) {
    constexpr auto buffer_size = 1024;
    char buffer[buffer_size];
    auto t = d.to_tm();
    size_t s = strftime(
        buffer, buffer_size, "%a %b %d %Y %H:%M:%S GMT+0000 (UTC)", &t);
    return context_->add_string(std::string(buffer, s));
  }
  jsstring operator()(jsarray_view a);

  jsstring operator()(jsobject_view o) { return "[object Object]"; }

private:
  eval_context *context_;
};

class to_string_visitor_for_array {
public:
  to_string_visitor_for_array(to_string_visitor *v) : visitor_(v) {}

  inline jsstring operator()(jsundefined u) { return ""; }
  inline jsstring operator()(jsnull u) { return ""; }
  inline jsstring operator()(bool b) { return (*visitor_)(b); }
  inline jsstring operator()(double d) { return (*visitor_)(d); }
  inline jsstring operator()(jsstring s) { return (*visitor_)(s); }
  inline jsstring operator()(jsdate d) { return (*visitor_)(d); }
  inline jsstring operator()(jsarray_view a) { return (*visitor_)(a); }
  inline jsstring operator()(jsobject_view o) { return (*visitor_)(o); }

private:
  to_string_visitor *visitor_;
};

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

}  // anonymous namespace

jsvalue to_string::evaluate(jsvalue v, eval_context *c) {
  to_string_visitor visitor(c);
  return jsvalue::string(v.visit(visitor));
}

}  // namespace operators
}  // namespace espress
