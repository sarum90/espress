#pragma once

// project includes:
#include "jsvalue.hpp"
#include "writer.hpp"

namespace espress {

// Write v as a JSON value.
void to_json(jsvalue v, writer *w);

// Write v as a JSON value, espcaping NaN, Inf, -Inf, undefined, and dates to
// custom structures.
void to_espress_json(jsvalue v, writer *w);

// Write v as a value that can be parsed by javascript.
//
// Identical to to_json, except for dates, which are written `new
// Date(ms_since_epoch)`
void to_js(jsvalue v, writer *w);
}
