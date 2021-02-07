#include "ModuleInfos.h"
#include "ModuleInfosDeserialize.h"

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

static DataSourceVariant parseDataSource(nlohmann::json source)
{
  std::string srcName;

  // If source is a string, change it to an empty object, for unified handling.
  if (source.is_string()) {
    source.get_to(srcName);
    source = nlohmann::json::object();
  } else if (source.is_object()) {
    source.at("type").get_to(srcName);
  } else {
    throw ModuleGridError("Module source should be a string or object.");
  }

  if (srcName == "cpu") {
    auto src = source.get<dataSources::Cpu>();
    if (src.cores.size() > 1)
      throw ModuleGridError("cpu source: More than one core specified");
    return src;
  }
  if (srcName == "mem") {
    return source.get<dataSources::Mem>();
  }
  if (srcName == "disk") {
    return source.get<dataSources::Disk>();
  }
  if (srcName == "net") {
    return source.get<dataSources::Net>();
  }
  throw ModuleGridError("Unknown data source " + srcName);
}

ModuleInfo::ModuleInfo(const nlohmann::json &jsonModule)
{
  dataSource = parseDataSource(jsonModule.at("src"));
  jsonModule.at("schema").get_to(schemaName);
  jsonModule.at("refresh").get_to(refreshDelay);
  jsonModule.at("row").get_to(row);
  jsonModule.at("column").get_to(column);
  if (jsonModule.contains("title"))
    jsonModule.at("title").get_to(title);
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

  for (const nlohmann::json &jsonRow : jsonGrid.at("rows")) {
    auto ratio = jsonRow.get<int>();
    rowWidths.push_back(ratio);
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