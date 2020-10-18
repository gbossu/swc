#include "ModuleGrid.h"
#include "ModuleInfos.h"
#include "CPUStatsReader.h"
#include "MemStatsReader.h"
#include "DiskStatsReader.h"
#include "Graph.h"
#include "json.hpp"
#include <fstream>
#include <QGridLayout>

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
  SchemaVisitor(const ModuleSize &modSize) : modSize(modSize) {}
  RetType operator()(const schemas::Line &schema) {
    return std::make_unique<LineGraph>(modSize, schema.points);
  }
  RetType operator()(const schemas::Bar &) {
    return std::make_unique<BarGraph>(modSize);
  }
private:
  const ModuleSize &modSize;
};

template <typename DataReader>
static std::unique_ptr<DataForwarderBase>
makeDefaultForwarder(ModuleBase &module, miliseconds refreshDelay)
{
  auto dataForwarder =
      std::make_unique<DataForwarder<DataReader>>(refreshDelay);
  dataForwarder->addModuleWithDefaultAction(module);
  return dataForwarder;
}

class DataSourceVisitor {
public:
  DataSourceVisitor(const ModuleInfo &moduleInfo, ModuleBase &module,
                    std::vector<std::unique_ptr<DataForwarderBase>> &forwarders)
      : refreshDelay(moduleInfo.getRefreshDelay()), module(module),
        forwarders(forwarders) {}
  void operator()(const dataSources::Cpu &) {
    forwarders.push_back(std::move(
        makeDefaultForwarder<utils::CpuUsage>(module, refreshDelay)));
  }
  void operator()(const dataSources::Mem &) {
    forwarders.push_back(std::move(
        makeDefaultForwarder<utils::MemStatsReader>(module, refreshDelay)));
  }
  void operator()(const dataSources::Disk &srcInfo) {
    if (srcInfo.path.empty()) {
      forwarders.push_back(std::move(
          makeDefaultForwarder<utils::DiskStatsReader>(module, refreshDelay)));
      return;
    }

    auto forwarder = std::make_unique<DataForwarder<utils::DiskStatsReader>>(
        refreshDelay);
    forwarder->addModule(
        module,
        [&srcInfo](const utils::DiskStatsReader &reader, ModuleBase &module) {
          module.add(reader.getDiskUsagePercent(srcInfo.path));
    });
    forwarders.push_back(std::move(forwarder));
  }
private:
  miliseconds refreshDelay;
  ModuleBase &module;
  std::vector<std::unique_ptr<DataForwarderBase>> &forwarders;
};

ModuleGrid::ModuleGrid(const ModuleSize &gridSize, QWidget *parent,
                       const std::string &settingsPath)
{
  // TODO: support more types of containers
  // TODO: support auto sizing

  gridInfo = createGridInfo(settingsPath);
  auto *gridWidget = new QWidget(parent);
  gridWidget->resize(gridSize.getOrSquare().toSize());
  auto *grid = new QGridLayout(gridWidget);

  const std::vector<int> &stretchFactors =
      gridInfo->getColumnStretchFactors();
  for (size_t idx = 0; idx < stretchFactors.size(); ++idx)
    grid->setColumnStretch(idx, stretchFactors[idx]);

  for (const ModuleInfo &moduleInfo : *gridInfo) {
    const ModuleSchema &schema =
        gridInfo->getSchema(moduleInfo.getSchemaName());

    // TODO: Modules might want to specify their preferred size.
    ModuleSize modSize;
    SchemaVisitor vis(modSize);
    std::unique_ptr<ModuleBase> module = std::visit(vis, schema.getVariant());
    grid->addWidget(module->getWidget(), moduleInfo.getRow(),
                    moduleInfo.getColumn());

    DataSourceVisitor srcVis(moduleInfo, *module, forwarders);
    std::visit(srcVis, moduleInfo.getDataSource());
    modules.push_back(std::move(module));
  }
}

ModuleGrid::~ModuleGrid()
{
  // Needed due to the forward-declaration of ModuleGridInfo
}

}