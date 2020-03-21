#include "AnimatedModuleContainer.h"
#include "Settings.h"
#include <CPUGraph.h>

AnimatedModuleContainer::AnimatedModuleContainer(
      std::unique_ptr<Settings> &&settings) :
  AnimatedContainer(std::move(settings))
{
  // Get the size
  QSize originalSize;
  const QString sizeType = this->settings->getString("container/size_type");
  if (sizeType == "absolute")
    originalSize = this->settings->getSize("container/size");
  else {
    qWarning("Error: size_type setting not recognized");
    return;
  }

  // Create a Cpu usage graph and embed it
  // TODO: support more modules
  module = new modules::CpuGraph(originalSize, this);
  setupContainerWidget(originalSize);
}

void AnimatedModuleContainer::animate() {
  // FIXME: find a better way to force Qt to export AnimatedContainer::animate
  // for the DBus.
  AnimatedContainer::animate();
}