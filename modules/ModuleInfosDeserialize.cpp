#include "ModuleInfosDeserialize.h"
#include "ModuleInfos.h"
#include "json.hpp"

template<typename T>
static void get_optional_to(const nlohmann::json &j,
                            const std::string &attrName, std::optional<T> &o)
{
  if (j.contains(attrName)) {
    o = j.at(attrName);
  } else {
    o = {};
  }
}

namespace modules {

namespace dataSources {


void from_json(const nlohmann::json &j, Cpu &o) {
  if (j.contains("cores")) {
    for (const nlohmann::json &core : j.at("cores")) {
      o.cores.push_back(core.get<unsigned>());
    }
  }
}

void from_json(const nlohmann::json &j, Mem &o) {
}

void from_json(const nlohmann::json &j, Disk &o) {
  get_optional_to(j, "path", o.path);
}

NLOHMANN_JSON_SERIALIZE_ENUM(Net::Direction, {
    {Net::UPLOAD, "upload"},
    {Net::DOWNLOAD, "download"}
})
void from_json(const nlohmann::json &j, Net &o) {
  j.at("interface").get_to(o.interface);
  get_optional_to(j, "direction", o.direction);
}

} // namespace dataSources

} // namespace modules