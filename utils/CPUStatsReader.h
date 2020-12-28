#pragma once

#include "DataReaderBase.h"
#include <vector>
#include <string>

namespace utils {
struct UsageTimes {
  unsigned busy = 0U;
  unsigned idle = 0U;

  UsageTimes operator-(const UsageTimes &rhs) const {
    return UsageTimes{busy - rhs.busy, idle - rhs.idle};
  }
};

class CpuUsage : public DataReaderBase {
public:
  enum ReadMode {
    Average,
    All,
  };

  CpuUsage(ReadMode mode = All);

  void update() override;

  /// Returns the average usage details of all cores since startup.
  const UsageTimes &getTotalAverageUsage() const {
    return averageUsage;
  }

  /// Returns a percentage of how busy the cpu was on average between the last
  /// two calls to update.
  float getAverageBusyPercent() const;

  /// Returns the usage details of coreIdx since startup.
  const UsageTimes &getTotalCoreUsage(unsigned coreIdx) const {
    return coresUsage[coreIdx];
  }

  /// Returns a percentage of how busy the given cpu core was between the last
  /// two calls to update.
  float getCoreBusyPercent(unsigned coreIdx) const;

  float getDefaultValue() const override {
    return getAverageBusyPercent();
  }

private:
  UsageTimes averageUsage;
  UsageTimes averageDiffUsage;
  std::vector<UsageTimes> coresUsage;
  std::vector<UsageTimes> coresDiffUsage;
};

}