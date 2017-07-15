#include "subprocess.hpp"

#include "test/common.hpp"

#include "util/read.hpp"
#include "util/write.hpp"

using namespace espress;
using namespace mettle;

test_suite<> subprocess_test("tests for subprocess", [](auto &_) {
  _.test("simple cat", []() {
    std::string_view in{"Hello world\n"};
    std::string out(in.size(), '\0');

    auto subp = subprocess::create({"cat"}, {});
    util::write_all(subp.std_in(), in);
    util::read_all(subp.std_out(), out);
    expect(out, equal_to(in));
  });

  _.test("simple echo", []() {
    std::string in{"Hello world"};
    std::string out(in.size(), '\0');

    auto subp = subprocess::create({"echo", "-n", in}, {});
    util::read_all(subp.std_out(), out);
    expect(out, equal_to(in));
  });

  _.test("bash echo env", []() {
    std::string in{"Hello world"};
    std::string out(in.size(), '\0');
    std::string varname{"MY_VAR_PROBABLY_UNSET"};

    auto subp = subprocess::create({"bash", "-c", "echo \"$" + varname + "\""},
                                   {{varname, in}});
    util::read_all(subp.std_out(), out);
    expect(out, equal_to(in));
  });

  _.test("invalid command", []() {
    expect(
        []() {
          auto subp =
              subprocess::create({"i-hope-this-isnt-a-command-in-path"}, {});
        },  // COVERAGE_MISS_OK
        thrown<util::assertion_error>());
  });

  _.test("invalid command with other exception", []() {
    expect(
        []() {
          auto subp =
              subprocess::create({"i-hope-this-isnt-a-command-in-path"}, {});
          throw std::runtime_error("cat");
        },
        thrown<std::runtime_error>("cat"));
  });
});
