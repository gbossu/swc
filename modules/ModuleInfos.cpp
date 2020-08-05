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
  } else if (typeName == "bar") {
    variant = schemas::Bar{};
  } else
    throw ModuleGridError("Unknown schema type " + typeName);
}

static DataSourceVariant parseDataSource(const std::string &srcString)
{
  size_t firstPar = srcString.find('(');
  std::string srcName = srcString.substr(0, firstPar);
  std::string args;
  if (firstPar != std::string::npos) {
    if (srcString.find(')') != srcString.size() - 1)
      throw ModuleGridError("Could not parse module source.");
    size_t argsLen = srcString.size() - srcName.size() - 2;
    args = srcString.substr(firstPar + 1, argsLen);
  }

  // TODO: handle args
  if (srcName == "cpu") {
    dataSources::Cpu src;
    return src;
  }
  if (srcName == "mem") {
    dataSources::Mem src;
    return src;
  }
  throw ModuleGridError("Unknown data source " + srcName);
}

ModuleInfo::ModuleInfo(const nlohmann::json &jsonModule)
{
  dataSource = parseDataSource(jsonModule.at("src").get<std::string>());
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