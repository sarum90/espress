#pragma once

#include <map>
#include <string_view>
#include <variant>
#include <vector>

namespace espress {

class jsarray;
class jsobject;

// Javascript Date representation.
class jsdate {
public:

private:
  uint64_t unix_;
};

// Javascript string representation. Does not own the underlying string.
class jsstring {
public:
  template<int N>
  jsstring(const char (&c)[N]): string_{c, N-1}{}

  std::string_view string_view() {
    return string_;
  }

private:
  std::string_view string_;
};

// Copy-by-value implementation of a javascript value.
//
// Javascript arrays, objects, and lists are not owned.
//
// TODO: Fancy memory annotations to help catch use-after-frees.
class jsvalue {
public:
  static jsvalue boolean(bool b) {return jsvalue(b);}
  static jsvalue number(double d) {return jsvalue(d);}
  static jsvalue string(jsstring s) {return jsvalue(s);}
  static jsvalue date(jsdate d) {return jsvalue(d);}
  static jsvalue array(jsarray* a) {return jsvalue(a);}
  static jsvalue object(jsobject* o) {return jsvalue(o);}

  // Visit this value, argument must be a visitor that can handle all types
  // that value_ can take.
  template <class T>
  inline decltype(auto) visit(T && t) {
    return std::visit(std::forward<T>(t), value_);
  }

private:
  template<class T>
  explicit jsvalue(T && t): value_(std::forward<T>(t)){};

  std::variant<
    bool,     // Javascript boolean
    double,   // Javascript number
    jsstring, // Javascript string
    jsdate,   // Javascript date
    jsarray*, // Javascript array
    jsobject* // Javascript object
  > value_;
};

// Representation of a javascript array.
class jsarray {
public:

private:
  std::vector<jsvalue> values_;
};

// Representation of a javascript object.
class jsobject {
public:

private:
  // TODO: Javascript objects can have prototypes, expand this to account for
  // that / handle method calls.
  std::map<jsstring, jsvalue> key_values_;
};

}  // namespace espress
