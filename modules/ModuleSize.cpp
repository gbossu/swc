#include "ModuleSize.h"

namespace modules
{

ModuleSize::ModuleSize(const QSize &size)
  : width(size.width()), height(size.height()) {}

ModuleSize::ModuleSize(const ModuleSize &modSize, float widthRatio, float heightRatio)
{
  if (modSize.width.has_value())
    width = *modSize.width * widthRatio;
  if (modSize.height.has_value())
    height = *modSize.height * heightRatio;
}

QSizeF ModuleSize::getOrSquare() const
{
  assert(width.has_value() || height.has_value());
  return QSizeF(width.value_or(*height), height.value_or(*width));
}
} // namespace modules
