#pragma once

namespace utils {

class DataReaderBase {
public:
  /// Called regularly by DataForwarders to update the values stored inside
  /// the DataReader.
  virtual void update() = 0;

  /// Returns a sensible default value for each DataReader.
  /// e.g. for cpu usage, it would be the average usage of all cores.
  virtual float getDefaultValue() const = 0;
};

}