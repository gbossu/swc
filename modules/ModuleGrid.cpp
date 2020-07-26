#include "ModuleGrid.h"
#include "ModuleInfos.h"
#include "CPUStatsReader.h"
#include "LineGraph.h"
#include "json.hpp"
#include <fstream>

namespace modules {

static std::unique_ptr<ModuleGridInfo>
createGridInfo(const std::string &jsonGridPath)
{
  std::ifstream settingsFile(jsonGridPath);
  if (!settingsFile.is_open())
    throw ModuleGridError("Can't open file.");
  nlohmann::json settings;
  settingsFile >> settings;
  return std::make_unique<ModuleGridInfo>(settings);
}

/// A visitor for schemas which will create the appropriate Module based on
/// the schema's attributes.
class SchemaVisitor {
using RetType = std::unique_ptr<ModuleBase>;
public:
  SchemaVisitor(QWidget *parent, const ModuleSize &modSize)
      : parent(parent), modSize(modSize) {}
  RetType operator()(const schemas::Line &schema) {
    return std::make_unique<LineGraph>(modSize, parent, schema.points);
  }
private:
  QWidget *parent;
  const ModuleSize &modSize;
};

ModuleGrid::ModuleGrid(const ModuleSize &size, QWidget *parent,
                       const std::string &settingsPath)
{
  // TODO: support more types of containers
  // TODO: support several modules

  gridInfo = createGridInfo(settingsPath);

  for (const ModuleInfo &moduleInfo : *gridInfo) {
    if (moduleInfo.getSourceName() != "cpu")
      throw ModuleGridError("Unknown module source");
    const ModuleSchema &schema =
        gridInfo->getSchema(moduleInfo.getSchemaName());
    miliseconds refreshRate = moduleInfo.getRefreshDelay();

    SchemaVisitor vis(parent, size);
    std::unique_ptr<ModuleBase> module = std::visit(vis, schema.getVariant());
    auto dataForwarder =
        std::make_unique<DataForwarder<utils::CpuUsage>>(refreshRate);
    dataForwarder->addModuleWithDefaultAction(*module);

    forwarders.push_back(std::move(dataForwarder));
    modules.push_back(std::move(module));
  }
}

ModuleGrid::~ModuleGrid()
{
  // Needed due to the forward-declaration of ModuleGridInfo
}

}