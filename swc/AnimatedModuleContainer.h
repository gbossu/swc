#pragma once

#include "AnimatedContainer.h"

namespace modules {
class ModuleGrid;
}

class AnimatedModuleContainer: public AnimatedContainer
{
  Q_OBJECT
public:
  AnimatedModuleContainer(std::unique_ptr<Settings> &&settings);
  ~AnimatedModuleContainer();

public slots:
  void animate() override;

private:
  /// The grid is responsible for placing and updating all the widgets in this
  /// container. Note that the widgets themselves are owned by this container
  /// via the Qt parenting system.
  std::unique_ptr<modules::ModuleGrid> grid;
};
