#pragma once

#include <string>
#include "DataReaderBase.h"

namespace utils
{

class DiskStatsReader : public DataReaderBase {
public:
  DiskStatsReader() = default;
  void update() override {}

  /// Returns the percentage of used space for the partition
  /// denoted by @ref path.
  float getDiskUsagePercent(const std::string &path) const;

  float getDefaultValue() const override {
    return getDiskUsagePercent("/");
  }

private:
  unsigned unit = 1000; //1000 <==> kB
};

} // namespace utils
