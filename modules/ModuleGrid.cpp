#include "ModuleGrid.h"
#include "CPUStatsReader.h"
#include "LineGraph.h"
#include "json.hpp"
#include <fstream>

namespace modules {

ModuleGrid::ModuleGrid(const QSize &size, QWidget *parent,
                       const std::string &settingsPath)
{
  // TODO: support more types of containers
  std::ifstream settingsFile(settingsPath);
  if (!settingsFile.is_open())
    throw ModuleGridError("Can't open file.");
  nlohmann::json settings;
  settingsFile >> settings;

  using ModuleMaker = std::function<std::unique_ptr<ModuleBase>(const QSize &moduleSize)>;
  std::map<std::string, ModuleMaker> moduleMakers;

  // Browse schemas and create lambda functions which create modules based on
  // their settings.
  for (const nlohmann::json &schema : settings.at("moduleschemas")) {
    if (schema.at("type").get<std::string>() != "line")
      throw ModuleGridError("Unknown schema type");
    auto numPoints = schema.at("points").get<size_t>();
    moduleMakers[schema.at("name").get<std::string>()] =
        [&](const QSize &moduleSize) {
          return std::make_unique<LineGraph>(moduleSize, parent, numPoints);
        };
  }

  for (const nlohmann::json &moduleInfo : settings.at("modules")) {
    if (moduleInfo.at("src").get<std::string>() != "cpu")
      throw ModuleGridError("Unknown module source");
    auto schemaName = moduleInfo.at("schema").get<std::string>();
    auto makerIt = moduleMakers.find(schemaName);
    if (makerIt == moduleMakers.end())
      throw ModuleGridError("Unknown schema " + schemaName);
    auto refreshRate = moduleInfo.at("refresh").get<miliseconds>();

    auto dataForwarder =
        std::make_unique<DataForwarder<utils::CpuUsage>>(refreshRate);
    std::unique_ptr<ModuleBase> module = makerIt->second(size);
    dataForwarder->addModuleWithDefaultAction(*module);

    forwarders.push_back(std::move(dataForwarder));
    modules.push_back(std::move(module));
  }
}

}