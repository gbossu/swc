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

  void operator()(const dataSources::Cpu &srcInfo) {
    if (srcInfo.cores.empty()) {
      forwarders.push_back(std::move(
          makeDefaultForwarder<utils::CpuUsage>(module, refreshDelay)));
      return;
    }

    auto forwarder =
        std::make_unique<DataForwarder<utils::CpuUsage>>(refreshDelay);
    auto callback = [&srcInfo](const utils::CpuUsage &reader, ModuleBase &module) {
      // TODO: support multiple cores
      unsigned coreIdx = srcInfo.cores.front();
      module.add(reader.getCoreBusyPercent(coreIdx));
    };
    forwarder->addModule(module, callback);
    forwarders.push_back(std::move(forwarder));
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
  gridInfo = createGridInfo(settingsPath);

  // TODO: support auto sizing of the grid based on the modules it contains
  auto *gridWidget = new QWidget(parent);
  gridWidget->resize(gridSize.getOrSquare().toSize());
  auto *grid = new QGridLayout(gridWidget);

  const std::vector<int> &rowStretchFactors =
      gridInfo->getRowsStretchFactors();
  for (size_t idx = 0; idx < rowStretchFactors.size(); ++idx)
    grid->setRowStretch(idx, rowStretchFactors[idx]);

  const std::vector<int> &columnStretchFactors =
      gridInfo->getColumnStretchFactors();
  for (size_t idx = 0; idx < columnStretchFactors.size(); ++idx)
    grid->setColumnStretch(idx, columnStretchFactors[idx]);

  for (const ModuleInfo &moduleInfo : *gridInfo) {
    const ModuleSchema &schema =
        gridInfo->getSchema(moduleInfo.getSchemaName());

    // TODO: Modules might want to specify their preferred size.
    // TODO: Modules might want to span over several rows/columns.
    ModuleSize modSize;
    SchemaVisitor vis(modSize);
    std::unique_ptr<ModuleBase> module = std::visit(vis, schema.getVariant());
    module->setTitle(moduleInfo.getTitle());
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