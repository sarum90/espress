#include "json.hpp"

#include <iostream>
#include <stdexcept>

#include "util/to_string.hpp"
#include "util/variant.hpp"
#include "util/write.hpp"

namespace espress {

namespace {

jsvalue parse_json_impl(auto& it, const auto& end, eval_context* c);

class json_visitor {
public:
  explicit json_visitor(writer* w) : writer_(w) {}

  void operator()(jsundefined u) { util::write_all(writer_, "null"); }

  void operator()(jsnull n) { util::write_all(writer_, "null"); }

  void operator()(double d) {
    if (std::isinf(d) || std::isnan(d)) {
      util::write_all(writer_, "null");
      return;
    }
    auto s = util::to_string(d);
    util::write_all(writer_, s);
  }

  void operator()(bool b) { util::write_all(writer_, b ? "true" : "false"); }

  void operator()(jsstring s) {
    util::write_all(writer_, "\"");
    std::string_view remaining = s.string_view();
    size_t escaped_pos = 0;
    while (remaining.size() > 0) {
      auto p = remaining.find_first_of("\"\\\n\t\b\r\f", escaped_pos);
      if (p == std::string_view::npos) {
        util::write_all(writer_, remaining);
        remaining = std::string_view{};
      } else {
        util::write_all(writer_, remaining.substr(0, p));
        if (remaining[p] == '\n') {
          util::write_all(writer_, "\\n");
          p++;
          // We haven't looked at anything beyond remaining[p].
          escaped_pos = 0;
        } else if (remaining[p] == '\t') {
          util::write_all(writer_, "\\t");
          p++;
          // We haven't looked at anything beyond remaining[p].
          escaped_pos = 0;
        } else if (remaining[p] == '\b') {
          util::write_all(writer_, "\\b");
          p++;
          // We haven't looked at anything beyond remaining[p].
          escaped_pos = 0;
        } else if (remaining[p] == '\r') {
          util::write_all(writer_, "\\r");
          p++;
          // We haven't looked at anything beyond remaining[p].
          escaped_pos = 0;
        } else if (remaining[p] == '\f') {
          util::write_all(writer_, "\\f");
          p++;
          // We haven't looked at anything beyond remaining[p].
          escaped_pos = 0;
        } else {
          util::write_all(writer_, "\\");
          // We've already escaped the first character (for \ and ")
          escaped_pos = 1;
        }
        remaining = remaining.substr(p);
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

  writer* writer_;
};

class js_visitor {
public:
  explicit js_visitor(writer* w) : visitor_(w) {}

  void operator()(jsundefined t) {
    util::write_all(visitor_.writer_, "undefined");
  }
  void operator()(jsnull t) { visitor_(t); }
  void operator()(double t) {
    util::write_all(visitor_.writer_, util::to_string(t));
  }
  void operator()(bool t) { visitor_(t); }
  void operator()(jsstring t) { visitor_(t); }

  void operator()(jsdate d) {
    util::write_all(visitor_.writer_, "new Date(");
    util::write_all(
        visitor_.writer_,
        std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
                           d.time_since_epoch())
                           .count()));
    util::write_all(visitor_.writer_, ")");
  }

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

class espress_json_visitor {
public:
  explicit espress_json_visitor(writer* w) : visitor_(w) {}

  void operator()(jsundefined t) {
    util::write_all(visitor_.writer_, "{\"__espress__\":\"undef\"}");
  }
  void operator()(jsnull t) { visitor_(t); }
  void operator()(double t) {
    if (std::isnan(t)) {
      util::write_all(visitor_.writer_, "{\"__espress__\":\"NaN\"}");
    } else if (std::isinf(t)) {
      if (t > 0) {
        util::write_all(visitor_.writer_, "{\"__espress__\":\"Inf\"}");
      } else {
        util::write_all(visitor_.writer_, "{\"__espress__\":\"-Inf\"}");
      }
    } else {
      util::write_all(visitor_.writer_, util::to_string(t));
    }
  }
  void operator()(bool t) { visitor_(t); }
  void operator()(jsstring t) { visitor_(t); }

  void operator()(jsdate d) {
    util::write_all(visitor_.writer_,
                    "{\"__espress__\":\"Date\",\"__value__\":");
    util::write_all(visitor_.writer_,
                    util::to_string(d.time_since_epoch().count()));
    util::write_all(visitor_.writer_, "}");
  }

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

void to_json(jsvalue v, writer* w) {
  json_visitor visitor(w);
  v.visit(visitor);
}

void to_espress_json(jsvalue v, writer* w) {
  espress_json_visitor visitor(w);
  v.visit(visitor);
}

void to_js(jsvalue v, writer* w) {
  js_visitor visitor(w);
  v.visit(visitor);
}

namespace {

void consume_whitespace(auto& it, const auto& end) {
  while (it != end &&
         (*it == ' ' || *it == '\n' || *it == '\t' || *it == '\r')) {
    it++;
  }
}

bool parse_matches(auto& it, const auto& end, std::string_view str) {
  auto it2 = str.begin();
  while (true) {
    if (it2 == str.end()) return true;
    if (it == end) return false;
    if (*it != *it2) return false;
    ++it;
    ++it2;
  }
}

double parse_number(auto& it, const auto& end) {
  double res = 0;
  while (it != end) {
    char c = *it;
    if (c <= '9' && c >= '0') {
      res *= 10;
      res += c - '0';
    } else {
      break;
    }
    ++it;
  }
  if (it != end && *it == '.') {
    ++it;
    double mult = 0.1;
    while (it != end) {
      char c = *it;
      if (c <= '9' && c >= '0') {
        double add = c - '0';
        res += add * mult;
        mult = mult / 10;
      } else {
        break;
      }
      ++it;
    }
  }
  return res;
}

std::string parse_string(auto& it, const auto& end) {
  std::string ret;
  util::eassert(*it == '"', "strings must start with '\"'");
  ++it;

  while (true) {
    util::eassert(it != end, "strings must end with '\"'");
    if (*it == '"') {
      ++it;
      return ret;
    }
    if (*it == '\\') {
      ++it;
      util::eassert(it != end, "unexpected end of string.");
      switch (*it) {
        case '"':
          ret += '"';
          break;
        case '\\':
          ret += '\\';
          break;
        case 'b':
          ret += '\b';
          break;
        case 'f':
          ret += '\f';
          break;
        case 'n':
          ret += '\n';
          break;
        case 't':
          ret += '\t';
          break;
        case 'r':
          ret += '\r';
          break;
        default:
          util::eassert(false, "invalid escape sequence in JSON string.");
      }
    } else {
      ret += *it;
    }
    ++it;
  }
}

jsarray_view parse_array(auto& it, const auto& end, eval_context* c) {
  util::eassert(*it == '[', "Arrays must start with '['.");
  ++it;
  auto& res = c->add_array();
  while (true) {
    consume_whitespace(it, end);
    util::eassert(it != end, "End of string before end of array.");
    if (*it == ']') {
      ++it;
      return jsarray_view{&res};
    }
    res.push_back(parse_json_impl(it, end, c));
    consume_whitespace(it, end);
    if (it != end && *it == ',') {
      ++it;
    }
  }
}

jsobject_view parse_object(auto& it, const auto& end, eval_context* c) {
  util::eassert(*it == '{', "Objects must start with '{'.");
  ++it;
  auto& res = c->add_object();
  while (true) {
    consume_whitespace(it, end);
    util::eassert(it != end, "End of string before end of object.");
    if (*it == '}') {
      ++it;
      return jsobject_view{&res};
    }
    auto key = c->add_string(parse_string(it, end));
    consume_whitespace(it, end);
    if (it != end && *it == ':') {
      ++it;
    }
    auto val = parse_json_impl(it, end, c);
    res.set(key, val);
    consume_whitespace(it, end);
    if (it != end && *it == ',') {
      ++it;
    }
  }
}

jsvalue parse_json_impl(auto& it, const auto& end, eval_context* c) {
  consume_whitespace(it, end);
  util::eassert(it != end, "Not enough string to json parser");
  if (*it == '-') {
    ++it;
    return jsvalue::number(-parse_number(it, end));
  } else if (*it <= '9' && *it >= '0') {
    return jsvalue::number(parse_number(it, end));
  } else if (*it == '"') {
    return jsvalue::string(c->add_string(parse_string(it, end)));
  } else if (*it == '[') {
    return jsvalue::array(parse_array(it, end, c));
  } else if (*it == '{') {
    return jsvalue::object(parse_object(it, end, c));
  } else if (parse_matches(it, end, "true")) {
    return jsvalue::boolean(true);
  } else if (parse_matches(it, end, "false")) {
    return jsvalue::boolean(false);
  } else if (parse_matches(it, end, "null")) {
    return jsvalue::null();
  }
  util::eassert(false, "Unknown character found during JSON parsing.");
  return jsvalue::null();  // COVERAGE_MISS_OK
}
}

jsvalue parse_json(std::string_view s, eval_context* c) {
  auto it = s.begin();
  auto ret = parse_json_impl(it, s.end(), c);
  return ret;
}

}  // namespace espress
