#include "util/assert.hpp"
#include "util/write.hpp"

#include <string.h>

namespace espress {

// This buffers before it writes.
//
// Note that for simplicity, this implementation will block rather than return
// partial writes when flushing the buffer.
template <class inner_writer>
class buffered_writer: public writer {
private:
  static constexpr int buffer_size = 4096;

public:
  template <class ... T>
  buffered_writer(T && ... args): inner_(std::forward<T>(args)...) {}

  int write(std::string_view s) final override {
    if (index_ + s.size() > buffer_size) {
      flush(); // blocks until flushed.
    }
    if (s.size() > buffer_size) {
      return inner_.write(s);
    }
    util::eassert(index_ + s.size() <= buffer_size, "Error in buffer logic");
    memcpy(&buffer_[index_], &s[0], s.size());
    index_ += s.size();
    return s.size();
  }

  void flush() {
    if (index_ > 0) {
      util::write_all(&inner_, std::string_view{&buffer_[0], index_});
      index_ = 0;
    }
  }

  inner_writer* inner() {
    return &inner_;
  }

private:
  inner_writer inner_;
  std::string_view::size_type index_ = 0;
  char buffer_[buffer_size];
};

} // namespace espress
