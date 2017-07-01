#pragma once

// project includes:
#include "writer.hpp"
#include "jsvalue.hpp"

namespace espress {

void to_json(jsvalue v, writer* w);

}
