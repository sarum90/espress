#include "pipe.hpp"

#include "test/common.hpp"

#include "util/write.hpp"
#include "util/read.hpp"

using namespace espress;
using namespace mettle;

test_suite<> pipe_tests("tests for pipe utilities", [](auto &_) {
  _.test("simple pipe usage", []() {
      espress::pipe p;
      auto w = p.writer();
      auto r = p.reader();
      std::string_view sv{"hello world"};
      util::write_all(&w, sv);
      std::string s(sv.size(), '\0');
      util::read_all(&r, s);
      expect(s, equal_to("hello world"));
  });

  _.test("simple moved pipe usage", []() {
      espress::pipe p1;
      espress::pipe p{std::move(p1)};
      auto w = p.writer();
      auto r = p.reader();
      std::string_view sv{"hello world"};
      util::write_all(&w, sv);
      std::string s(sv.size(), '\0');
      util::read_all(&r, s);
      expect(s, equal_to("hello world"));
  });

  _.test("simple moved pipe usage 2", []() {
      espress::pipe p1;
      espress::pipe p = std::move(p1);
      auto w = p.writer();
      auto r = p.reader();
      std::string_view sv{"hello world"};
      util::write_all(&w, sv);
      std::string s(sv.size(), '\0');
      util::read_all(&r, s);
      expect(s, equal_to("hello world"));
  });

  _.test("invalid moved pipe", []() {
      espress::pipe p1;
      espress::pipe p{std::move(p1)};
      expect([&](){p1.writer();},
             thrown<std::runtime_error>());
  });

  _.test("invalid moved pipe two", []() {
      espress::pipe p1;
      espress::pipe p;
      p = std::move(p1);
      expect([&](){p1.writer();},
             thrown<std::runtime_error>());
  });
});
