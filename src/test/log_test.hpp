#pragma once

#include <iostream>
#include <sstream>

class log_test: public std::ostringstream {
public:

  ~log_test() {
    if (std::uncaught_exception()) {
      std::cerr << str();
    }
  }

private:
};

