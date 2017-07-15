#pragma once

#include "operators/binary_operator.hpp"

#include "jsvalue.hpp"

namespace espress {
namespace operators {

class plus : public binary_operator<plus> {
public:
  inline static double evaluate(double l, double r, eval_context *c) {
    return l + r;
  }
};

}  // namespace operators
}  // namespace espress
