#pragma once

#include <variant>

namespace espress {
namespace util {

// Simple utility to pass a bunch of overloaded lambdas to std::visit.
//
// Intended use:
//
//   std::visit(
//    util::overloaded{
//      [](bool b){ return b ? "true" : "false"; },
//      [](double d){ return std::to_string(d); },
//      [](auto c){ throw std::runtime_error("unhandled type."}
//    }, variant_object);
//
//
template <class... Ts>
struct overloaded : Ts... {
  using Ts::operator()...;
};
template <class... Ts>
overloaded(Ts...)->overloaded<Ts...>;

}  // namespace util
}  // namespace espress
