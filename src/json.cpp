#include "json.hpp"

#include <stdexcept>

#include "util/variant.hpp"
#include "util/write.hpp"

namespace espress {

namespace {

}  // anonymous namespace

void to_json(jsvalue v, writer* w) {
  v.visit(
    util::overloaded{
      [w=w](double d){
        auto s = std::to_string(d);
        util::write_all(w, s);
      },
      [w=w](bool b){
        util::write_all(w, b ? "true": "false");
      },
      [w=w](jsstring s){
        util::write_all(w, "\"");
        std::string_view remaining = s.string_view();
        size_t escaped_pos = 0;
        while(remaining.size() > 0) {
          auto p = remaining.find_first_of("\"\\", escaped_pos);
          if (p == std::string_view::npos) {
            util::write_all(w, remaining);
            remaining = std::string_view{};
          } else {
            util::write_all(w, remaining.substr(0, p));
            util::write_all(w, "\\");
            remaining = remaining.substr(p);
            // We've already escaped the first character now.
            escaped_pos = 1;
          }
        }
        util::write_all(w, "\"");
      },
    }
  );
}

}  // namespace espress
