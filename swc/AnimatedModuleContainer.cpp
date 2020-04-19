#include "AnimatedModuleContainer.h"
#include "Settings.h"
#include "ModuleGrid.h"

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

  // Create all the modules and embed them in this container.
  grid = std::make_unique<modules::ModuleGrid>(originalSize, this);
  setupContainerWidget(originalSize);
}

AnimatedModuleContainer::~AnimatedModuleContainer() = default;

void AnimatedModuleContainer::animate() {
  // FIXME: find a better way to force Qt to export AnimatedContainer::animate
  // for the DBus.
  AnimatedContainer::animate();
}