#pragma once

namespace espress {
namespace operators {

template <class sub>
class binary_operator {
public:
  constexpr static bool is_binary_operator = true;
};

}  // namespace operators
}  // namespace espress
