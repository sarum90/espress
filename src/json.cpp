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
    long int i = d;
    if (i == d) {
      auto s = std::to_string(i);
      util::write_all(writer_, s);
    } else {
      auto s = std::to_string(d);
      util::write_all(writer_, s);
    }
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

  void operator()(jsdate d) {
    auto date_time = d.to_tm();
    auto ms = d.time_since_epoch() % std::chrono::seconds{1};
    constexpr size_t buff_size = 128;
    char buffer[buff_size];
    size_t bytes = snprintf(buffer,
                            buff_size,
                            "\"%d-%02d-%02dT%02d:%02d:%02d.%03ldZ\"",
                            date_time.tm_year + 1900,
                            date_time.tm_mon + 1,
                            date_time.tm_mday,
                            date_time.tm_hour,
                            date_time.tm_min,
                            date_time.tm_sec,
                            ms.count());
    util::eassert(bytes >= 0 && bytes < buff_size, "Error running snprintf.");
    util::write_all(writer_, {buffer, bytes});
  }

  void operator()(jsarray_view a) {
    util::write_all(writer_, "[");
    bool first = true;
    for (auto js : a) {
      if (first) {
        first = false;
      } else {
        util::write_all(writer_, ",");
      }
      js.visit(*this);
    }
    util::write_all(writer_, "]");
  }

  void operator()(jsobject_view o) {
    util::write_all(writer_, "{");
    bool first = true;
    for (auto[k, v] : o) {
      if (first) {
        first = false;
      } else {
        util::write_all(writer_, ",");
      }
      (*this)(k);
      util::write_all(writer_, ":");
      v.visit(*this);
    }
    util::write_all(writer_, "}");
  }

  writer *writer_;
};

class js_visitor {
public:
  explicit js_visitor(writer *w) : visitor_(w) {}

  void operator()(jsdate d) {
    util::write_all(visitor_.writer_, "new Date(");
    util::write_all(
        visitor_.writer_,
        std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
                           d.time_since_epoch())
                           .count()));
    util::write_all(visitor_.writer_, ")");
  }

  void operator()(double t) { visitor_(t); }
  void operator()(bool t) { visitor_(t); }
  void operator()(jsstring t) { visitor_(t); }

  void operator()(jsarray_view a) {
    util::write_all(visitor_.writer_, "[");
    bool first = true;
    for (auto js : a) {
      if (first) {
        first = false;
      } else {
        util::write_all(visitor_.writer_, ",");
      }
      js.visit(*this);
    }
    util::write_all(visitor_.writer_, "]");
  }

  void operator()(jsobject_view o) {
    util::write_all(visitor_.writer_, "{");
    bool first = true;
    for (auto[k, v] : o) {
      if (first) {
        first = false;
      } else {
        util::write_all(visitor_.writer_, ",");
      }
      (*this)(k);
      util::write_all(visitor_.writer_, ":");
      v.visit(*this);
    }
    util::write_all(visitor_.writer_, "}");
  }

private:
  json_visitor visitor_;
};

}  // anonymous namespace

void to_json(jsvalue v, writer *w) {
  json_visitor visitor(w);
  v.visit(visitor);
}

void to_js(jsvalue v, writer *w) {
  js_visitor visitor(w);
  v.visit(visitor);
}

}  // namespace espress
