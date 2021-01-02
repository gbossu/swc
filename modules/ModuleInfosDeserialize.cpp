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