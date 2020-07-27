#include "ModuleInfos.h"

namespace modules {

ModuleSchema::ModuleSchema(const nlohmann::json &jsonSchema)
  : name(jsonSchema.at("name").get<std::string>())
{
  auto typeName = jsonSchema.at("type").get<std::string>();
  if (typeName == "line") {
    auto numPoints = jsonSchema.at("points").get<unsigned>();
    auto lineWidth = jsonSchema.at("lineWidth").get<unsigned>();
    variant = schemas::Line{numPoints, lineWidth};
  }
  else
    throw ModuleGridError("Unknown schema type " + typeName);
}

ModuleInfo::ModuleInfo(const nlohmann::json &jsonModule)
{
  jsonModule.at("src").get_to(sourceName);
  jsonModule.at("schema").get_to(schemaName);
  jsonModule.at("refresh").get_to(refreshDelay);
  jsonModule.at("row").get_to(row);
  jsonModule.at("column").get_to(column);
}

ModuleGridInfo::ModuleGridInfo(const nlohmann::json &jsonGrid)
{
  for (const nlohmann::json &jsonSchema : jsonGrid.at("schemas")) {
    ModuleSchema schema(jsonSchema);
    moduleSchemas.emplace(schema.getName(), std::move(schema));
  }

  for (const nlohmann::json &jsonModule : jsonGrid.at("modules")) {
    modules.emplace_back(jsonModule);
    maxRow = std::max(maxRow, modules.back().getRow());
  }

  for (const nlohmann::json &jsonColumn : jsonGrid.at("columns")) {
    auto ratio = jsonColumn.get<int>();
    columnWidths.push_back(ratio);
  }
}

const ModuleSchema &
ModuleGridInfo::getSchema(const std::string &schemaName) const
{
  try {
    return moduleSchemas.at(schemaName);
  } catch (std::out_of_range &) {
    throw ModuleGridError("Unknown schema " + schemaName);
  }
}

ModuleGridInfo::ConstModuleIt ModuleGridInfo::begin() const
{
  return modules.begin();
}
ModuleGridInfo::ConstModuleIt ModuleGridInfo::end() const
{
  return modules.end();
}

}