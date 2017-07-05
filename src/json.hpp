#pragma once

// project includes:
#include "jsvalue.hpp"
#include "writer.hpp"

namespace espress {

void to_json(jsvalue v, writer *w);
}
