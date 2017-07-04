#include "json.hpp"

#include <stdexcept>

#include "util/variant.hpp"
#include "util/write.hpp"

namespace espress {

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
      [](auto v){ throw std::runtime_error("Unhandled type passed to to_json."); },
    }
  );
}


}
