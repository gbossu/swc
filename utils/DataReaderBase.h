#pragma once

#include <optional>

namespace utils {

// TODO: make template to support other types than float, and create
// DataReaderVariant, which could just use dynamic_casts.
class DataReaderBase {
public:
  /// Called regularly by DataForwarders to update the values stored inside
  /// the DataReader.
  virtual void update() = 0;

  /// Returns a sensible default value for each DataReader.
  /// e.g. for cpu usage, it would be the average usage of all cores.
  /// TODO: remove? no default value?
  virtual float getDefaultValue() const = 0;

  /// Returns the maximum value which this reader can ever provide.
  virtual std::optional<float> getMaxValue() const {
    return {};
  }
};

}