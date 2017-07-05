#include <unistd.h>

#include "buffered_writer.hpp"
#include "json.hpp"
#include "jsvalue.hpp"

using namespace espress;

int main(int argc, char **argv) {
  jsvalue v = jsvalue::boolean(true);
  buffered_writer<file_writer> w(STDOUT_FILENO);
  to_json(v, &w);
  util::write_all(&w, "\n");
  w.flush();
  return 0;
}
