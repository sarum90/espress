#pragma once

#include "jsvalue.hpp"

#include <memory>

namespace espress {

class eval_context {
public:
  eval_context() {};

  const jsstring add_string(std::string s) {
    strings_.push_back(std::make_unique<std::string>(std::move(s)));
    std::string_view sv(*strings_[strings_.size()-1]);
    return sv;
  }

  jsarray& add_array() {
    arrays_.push_back(std::make_unique<jsarray>());
    return *arrays_[arrays_.size()-1];
  }

  jsobject& add_object() {
    objects_.push_back(std::make_unique<jsobject>());
    return *objects_[objects_.size()-1];
  }

private:
  // TODO:better memory management. This will make a ton of tiny objects all
  // over the place.
  std::vector<std::unique_ptr<std::string>> strings_;
  std::vector<std::unique_ptr<jsarray>> arrays_;
  std::vector<std::unique_ptr<jsobject>> objects_;
};

}  // namespace espress
