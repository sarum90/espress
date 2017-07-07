#include "buffered_writer.hpp"

#include "test/common.hpp"

using namespace espress;
using namespace mettle;

struct simple_fake_writer : public writer {
  int write(std::string_view s) final override {
    written_data += s;
    write_calls++;
    return s.size();
  }

  std::string written_data;
  int write_calls = 0;
};

test_suite<> buffered_writer_tests("bufferred writer suite", [](auto &_) {
  _.test("reduces writes", []() {
    buffered_writer<simple_fake_writer> bw;
    simple_fake_writer sfw;
    constexpr int write_count = 1e6;

    auto index_to_char = [](int i) -> char { return 'a' + (i % 26); };

    for (int i = 0; i < write_count; i++) {
      std::string s{index_to_char(i)};
      bw.write(s);
      sfw.write(s);
    }
    bw.flush();
    expect(bw.inner()->written_data.size(), equal_to(sfw.written_data.size()));
    expect(bw.inner()->written_data, equal_to(sfw.written_data));
    expect(sfw.write_calls, equal_to(write_count));
    expect(bw.inner()->write_calls, less(sfw.write_calls));
  });

  _.test("Handles writes of all sizes", []() {
    buffered_writer<simple_fake_writer> bw;
    std::string separator = "_-_";
    std::string small = "a";
    std::string mid = "horsecat";
    std::string large;
    for (int i = 0; i < 1e6; i++) {
      large += small + mid;
    }
    bw.write(small);
    bw.write(separator);
    bw.write(mid);
    bw.write(separator);
    bw.write(large);
    bw.write(separator);
    bw.write(mid);
    bw.write(separator);
    bw.write(small);
    bw.flush();
    expect(bw.inner()->written_data,
           equal_to(small + separator + mid + separator + large + separator +
                    mid + separator + small));
  });

});
