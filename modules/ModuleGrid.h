#pragma once

#include <QWidget>
#include <vector>
#include <memory>
#include <string>
#include "ModuleUtils.h"
#include "ModuleBase.h"
#include "ModuleSize.h"

namespace modules {

class ModuleGridInfo;

/// The @ref ModuleGrid is responsible for creating and placing all modules
/// at the right position in an @ref AnimatedModuleContainer, and with a
/// proper size.
/// For modules which need a flow of data as input, it will also setup the
/// corresponding DataForwarders.
class ModuleGrid {
public:

  /// @param size Known size parameters as specified in @ref Settings.
  /// @param parent The widget where modules will be added.
  /// @param settingsPath Path to a JSON describing the grid.
  ModuleGrid(const ModuleSize &size, QWidget *parent,
             const std::string &settingsPath);
  ~ModuleGrid();

private:
  std::vector<std::unique_ptr<DataForwarderBase>> forwarders;
  std::vector<std::unique_ptr<ModuleBase>> modules;
  std::unique_ptr<ModuleGridInfo> gridInfo;
};

}