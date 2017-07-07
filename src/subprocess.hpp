#include "reader.hpp"
#include "writer.hpp"

namespace espress {

class subprocess {
public:
  // writer to write to stdin.
  writer* std_in();

  // reader to read from stdout.
  reader* std_out();

private:
};

}  // namespace espress
