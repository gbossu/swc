#pragma once

// TODO: properly integrate nlohmann::json and use forward declarations.
#include "json.hpp"

namespace modules {

namespace dataSources {

struct Cpu;
struct Mem;
struct Disk;
struct Net;

void from_json(const nlohmann::json &j, Cpu &o);
void from_json(const nlohmann::json &j, Mem &o);
void from_json(const nlohmann::json &j, Disk &o);
void from_json(const nlohmann::json &j, Net &o);

} // namespace dataSources

} // namespace modules