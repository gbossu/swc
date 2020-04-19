#include "ModuleGrid.h"
#include "CPUStatsReader.h"
#include "LineGraph.h"

namespace modules {

ModuleGrid::ModuleGrid(const QSize &size, QWidget *parent)
{
  // TODO: support more types of containers
  auto cpuStatsForwarder =
      std::make_unique<DataForwarder<utils::CpuUsage>>(1000);
  auto cpuGraph = std::make_unique<LineGraph>(size, parent);
  cpuStatsForwarder->addModuleWithDefaultAction(*cpuGraph);

  forwarders.push_back(std::move(cpuStatsForwarder));
  modules.push_back(std::move(cpuGraph));
}

}