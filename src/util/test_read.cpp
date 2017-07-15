#include "util/read.hpp"

#include "test/common.hpp"
#include <iostream>

using namespace espress;
using namespace mettle;

struct bad_reader : public reader {
  int read(read_buffer r) final override { return 0; }
};

class string_reader : public reader {
public:
  string_reader(std::string s): str_(s){}

  int read(read_buffer r) final override {
    if (r.size() > str_.size()) {
      size_t sz = str_.size();
      memcpy(r.data(), &str_[0], sz);
      str_ = "";
      return sz;
    }
    memcpy(r.data(), &str_[0], r.size());
    str_ = str_.substr(r.size());
    return r.size();
  }

private:
  std::string str_;
};


test_suite<> util_read_tests("read util tests", [](auto &_) {
  _.test("raises error on bad read", []() {
    bad_reader bad;
    std::string s(100, '\0');
    expect([&]() { util::read_all(&bad, s); }, thrown<std::runtime_error>());
  });

  _.test("raises error on char not found read", []() {
    bad_reader bad;
    std::string s(100, '\0');
    expect([&]() { util::read_to(&bad, '\n', s); }, thrown<std::runtime_error>());
  });

  _.test("Returns n if buffer too small", []() {
    string_reader sr(std::string(std::string(150, '0') + "1"));
    std::string s(100, '\0');
    expect(util::read_to(&sr, '1', s), equal_to(100));
    expect(util::read_to(&sr, '1', s), equal_to(51));
  });
});
