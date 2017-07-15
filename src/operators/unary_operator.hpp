#pragma once

#include "jsvalue.hpp"

namespace espress {
namespace operators {

template <class sub>
class unary_operator {
public:
  constexpr static bool is_unary_operator = true;
  constexpr static bool is_binary_operator = false;
};

}  // namespace operators
}  // namespace espress
