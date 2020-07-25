#pragma once

#include <QSize>
#include <QSizeF>
#include <optional>

namespace modules {

struct ModuleSize {
  ModuleSize() = default;
  ModuleSize(const ModuleSize&) = default;
  ModuleSize(const QSize &size);
  ModuleSize(const ModuleSize &modSize, float widthRatio, float heightRatio);

  /// Return a QSize based on the width and height if they are both present.
  /// Otherwise, uses the height or width which is present as the missing
  /// dimension as well.
  ///
  /// \pre One dimension is present.
  QSizeF getOrSquare() const;

  std::optional<float> width;
  std::optional<float> height;
};

}