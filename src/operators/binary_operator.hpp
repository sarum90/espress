#pragma once

namespace espress {
namespace operators {

template <class sub>
class binary_operator {
public:
  constexpr static bool is_binary_operator = true;

  template <class L, class R>
  inline static jsunknown evaluate(L l, R r, eval_context* c) {
    return jsunknown{};
  }
};

}  // namespace operators
}  // namespace espress
