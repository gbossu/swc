#pragma once

#include <QWidget>
#include <vector>
#include <memory>
#include "ModuleUtils.h"

namespace modules {

class ModuleGrid {
public:
  ModuleGrid(const QSize &size, QWidget *parent);

private:
  std::vector<std::unique_ptr<DataForwarderBase>> forwarders;
  std::vector<std::unique_ptr<ModuleBase>> modules;
};

}