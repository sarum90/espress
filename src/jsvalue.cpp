#include "jsvalue.hpp"

namespace espress {

jsarray_view::const_iterator jsarray_view::begin() const {
  return array_->begin();
}

jsarray_view::const_iterator jsarray_view::end() const { return array_->end(); }

bool jsarray_view::operator==(const jsarray_view& other) const {
  const_iterator a = begin();
  const_iterator b = other.begin();
  for (; a != end() && b != other.end(); ++a, ++b) {
    if (!(*a == *b)) {
      return false;
    }
  }
  if (a != end() || b != other.end()) {
    return false;
  }
  return true;
}

jsobject_view::const_iterator jsobject_view::begin() const {
  return object_->begin();
}

jsobject_view::const_iterator jsobject_view::end() const {
  return object_->end();
}

bool jsobject_view::operator==(const jsobject_view& other) const {
  const_iterator a = begin();
  const_iterator b = other.begin();
  for (; a != end() && b != other.end(); ++a, ++b) {
    if (!(*a == *b)) {
      return false;
    }
  }
  if (a != end() || b != other.end()) {
    return false;
  }
  return true;
}

}  // namespace espress
