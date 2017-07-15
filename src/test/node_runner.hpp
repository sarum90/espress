#pragma once

#include "json.hpp"
#include "subprocess.hpp"
#include "util/read.hpp"
#include "util/write.hpp"

// TODO: This seems like it might be pretty bad...
#ifdef COVERAGE
#define ESPRESS_JSEVAL_PATH "../../tools/jseval.js"
#else
#define ESPRESS_JSEVAL_PATH "tools/jseval.js"
#endif

namespace espress {

class node_runner {
public:
  node_runner()
      : node_(subprocess::create({"node", ESPRESS_JSEVAL_PATH},
                                 {{"TZ", "Etc/UTC"}})) {}

  //. N.B. only use in tests, has a limit on size of output.
  std::string run(std::string_view s) {
    jsobject o;
    std::string script = "(function() {return (";
    script += s;
    script += ");})()";
    o.set("script", jsvalue::string(std::string_view(script)));
    to_json(jsvalue::object(&o), node_.std_in());
    util::write_all(node_.std_in(), "\n");
    std::string b(40960, '\0');
    size_t bts = util::read_to(node_.std_out(), '\n', b);
    return b.substr(0, bts - 1);
  }

private:
  subprocess node_;
};

}  // namespace espress

#undef ESPRESS_JSEVAL_PATH
