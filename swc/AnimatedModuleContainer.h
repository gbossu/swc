#pragma once

#include "AnimatedContainer.h"

namespace modules {
class CpuGraph;
}

class AnimatedModuleContainer: public AnimatedContainer
{
  Q_OBJECT
public:
  AnimatedModuleContainer(std::unique_ptr<Settings> &&settings);

public slots:
  void animate() override;

private:
  // TODO: support more types of containers
  /// The widget which is embedded in this container. Note that it is also
  /// owned by this container via Qt parent system.
  modules::CpuGraph *module;
};
