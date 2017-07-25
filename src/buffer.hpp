#pragma once

#include "writer.hpp"

namespace espress {

// TODO: make more efficient.
class buffer : public writer {
public:
  int write(std::string_view s) override final {
    data_ += s;
    return s.size();
  }

  std::string_view string() { return data_; }

private:
  std::string data_;
};
}
