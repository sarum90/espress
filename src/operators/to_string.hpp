#pragma once

#include "eval_context.hpp"
#include "operators/unary_operator.hpp"

namespace espress {
namespace operators {

class to_string : public unary_operator<to_string> {
public:
  // Note: assumes timezone is UTC.
  static jsvalue evaluate(jsvalue v, eval_context* c);
};

}  // namespace operators
}  // namespace espress
