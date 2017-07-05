
#include <mettle.hpp>

extern "C" {
#ifdef COVERAGE
extern void __gcov_flush(void);
#endif
}

namespace espress {

template <typename... Fixtures>
class test_suite {
public:
  template <typename... Args>
  test_suite(Args &&... args)
      : suite("Espress test suite", [&](auto &_) {
#ifdef COVERAGE
          _.teardown([]() { __gcov_flush(); });
#endif
          mettle::subsuite<Fixtures...>(_, std::forward<Args>(args)...);
        }) {
  }

private:
  mettle::suite<> suite;
};

}  // namespace espress
