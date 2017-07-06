#include "jsvalue.hpp"

namespace espress {

jsarray_view::const_iterator jsarray_view::begin() const {
  return array_->begin();
}

jsarray_view::const_iterator jsarray_view::end() const {
  return array_->end();
}

jsobject_view::const_iterator jsobject_view::begin() const {
  return object_->begin();
}

jsobject_view::const_iterator jsobject_view::end() const {
  return object_->end();
}

} // namespace espress
