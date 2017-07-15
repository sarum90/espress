#pragma once

#include <chrono>
#include <map>
#include <string_view>
#include <variant>
#include <vector>
#include <iostream>

#include "util/assert.hpp"

namespace espress {

class jsarray;
class jsobject;
class jsvalue;

struct jsundefined {
  bool operator==(const jsundefined& v) const { return true; }
};

struct jsnull {
  bool operator==(const jsnull& v) const { return true; }
};

// Javascript string representation. Does not own the underlying string.
class jsstring {
public:
  template <int N>
  jsstring(const char (&c)[N]) : string_{c, N - 1} {}

  jsstring(std::string_view s) : string_(s) {}

  std::string_view string_view() { return string_; }

  inline bool operator<(const jsstring &other) const {
    return string_ < other.string_;
  }

  inline bool operator==(const jsstring& other) const { return string_ == other.string_; }
  inline bool operator==(const std::string& other) const { return string_ == other; }
  inline bool operator==(const std::string_view& other) const { return string_ == other; }

private:
  friend std::ostream& operator<<(std::ostream& os, const jsstring& obj);
  std::string_view string_;
};

inline std::ostream& operator<<(std::ostream& os, const jsstring& obj) {
  return (os << obj.string_);
}


class jsarray_view {
public:
  typedef std::vector<jsvalue>::iterator iterator;
  typedef std::vector<jsvalue>::const_iterator const_iterator;

  jsarray_view(jsarray *array) : array_(array) {}

  const_iterator begin() const;
  const_iterator end() const;

  bool operator==(const jsarray_view& other) const;

private:
  jsarray *array_;
};

class jsobject_view {
public:
  typedef std::map<jsstring, jsvalue>::const_iterator const_iterator;

  jsobject_view(jsobject *object) : object_(object) {}

  const_iterator begin() const;
  const_iterator end() const;

  bool operator==(const jsobject_view& other) const;

private:
  jsobject *object_;
};

// Javascript Date representation.
//
// N.B. Only supports UTC.
class jsdate {
public:
  jsdate(std::chrono::milliseconds time_since_epoch)
      : time_since_epoch_(time_since_epoch) {}

  inline ::tm to_tm() {
    ::tm res;
    ::time_t t = to_time_t();
    util::eassert(gmtime_r(&t, &res) != nullptr,
                  "Failed to encode jsdate as struct tm.");
    return res;
  }

  inline time_t to_time_t() {
    return std::chrono::system_clock::to_time_t(to_time_point());
  }

  inline std::chrono::system_clock::time_point to_time_point() {
    return std::chrono::system_clock::time_point{} + time_since_epoch_;
  }

  inline std::chrono::milliseconds time_since_epoch() {
    return time_since_epoch_;
  }

  inline bool operator==(const jsdate& other) const {
    return time_since_epoch_ == other.time_since_epoch_;
  }

private:
  std::chrono::milliseconds time_since_epoch_;
};

// Copy-by-value implementation of a javascript value.
//
// Javascript arrays, objects, and lists are not owned.
//
// TODO: Fancy memory annotations to help catch use-after-frees.
class jsvalue {
public:
  static jsvalue undefined() { return jsvalue(jsundefined{}); }
  static jsvalue null() { return jsvalue(jsnull{}); }
  static jsvalue boolean(bool b) { return jsvalue(b); }
  static jsvalue number(double d) { return jsvalue(d); }
  static jsvalue string(jsstring s) { return jsvalue(s); }
  static jsvalue date(jsdate d) { return jsvalue(d); }
  static jsvalue array(jsarray_view a) { return jsvalue(a); }
  static jsvalue object(jsobject_view o) { return jsvalue(o); }

  inline bool operator==(const jsvalue& v) const {
    return value_ == v.value_;
  }

  // Visit this value, argument must be a visitor that can handle all types
  // that value_ can take.
  template <class T>
  inline decltype(auto) visit(T &&t) {
    return std::visit(std::forward<T>(t), value_);
  }

  jsstring as_string() const {
    return std::get<jsstring>(value_);
  }

private:
  template <class T>
  explicit jsvalue(T &&t) : value_(std::forward<T>(t)){};

  std::variant<jsundefined,   // Javascript undefined
               jsnull,        // Javascript null
               bool,          // Javascript boolean
               double,        // Javascript number
               jsstring,      // Javascript string
               jsdate,        // Javascript date
               jsarray_view,  // Javascript array
               jsobject_view  // Javascript object
               >
      value_;
};
static_assert(sizeof(jsvalue) == 24, "jsvalue should not be larger than 24 bytes.");

// Representation of a javascript array.
class jsarray {
public:
  typedef jsarray_view::iterator iterator;
  typedef jsarray_view::const_iterator const_iterator;

  void push_back(jsvalue v) { values_.push_back(v); }

  const_iterator begin() const { return values_.begin(); }

  const_iterator end() const { return values_.end(); }

private:
  std::vector<jsvalue> values_;
};

// Representation of a javascript object.
class jsobject {
public:
  typedef jsobject_view::const_iterator const_iterator;

  void set(jsstring s, jsvalue v) { key_values_.insert({s, v}); }

  const_iterator begin() const { return key_values_.begin(); }

  const_iterator end() const { return key_values_.end(); }

private:
  // TODO: Javascript objects can have prototypes, expand this to account for
  // that / handle method calls.
  std::map<jsstring, jsvalue> key_values_;
};

}  // namespace espress
