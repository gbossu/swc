#pragma once

// TODO: properly integrate nlohmann::json and use forward declarations.
#include "json.hpp"

namespace modules {

namespace dataSources {

struct Net;
void from_json(const nlohmann::json &j, Net &o);

} // namespace dataSources

} // namespace modules