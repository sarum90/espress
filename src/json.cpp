#include "json.hpp"

#include <stdexcept>

#include "util/variant.hpp"
#include "util/write.hpp"

namespace espress {

namespace {

class json_visitor {
public:
  explicit json_visitor(writer *w) : writer_(w) {}

  void operator()(double d) {
    auto s = std::to_string(d);
    util::write_all(writer_, s);
  }

  void operator()(bool b) { util::write_all(writer_, b ? "true" : "false"); }

  void operator()(jsstring s) {
    util::write_all(writer_, "\"");
    std::string_view remaining = s.string_view();
    size_t escaped_pos = 0;
    while (remaining.size() > 0) {
      auto p = remaining.find_first_of("\"\\", escaped_pos);
      if (p == std::string_view::npos) {
        util::write_all(writer_, remaining);
        remaining = std::string_view{};
      } else {
        util::write_all(writer_, remaining.substr(0, p));
        util::write_all(writer_, "\\");
        remaining = remaining.substr(p);
        // We've already escaped the first character now.
        escaped_pos = 1;
      }
    }
    util::write_all(writer_, "\"");
  }

  template <class T>
  void operator()(T &&t) {
    throw std::runtime_error("Unhandled visit type");
  }

private:
  writer *writer_;
};

}  // anonymous namespace

void to_json(jsvalue v, writer *w) {
  json_visitor visitor(w);
  v.visit(visitor);
}

}  // namespace espress
