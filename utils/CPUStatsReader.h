#pragma once

#include <vector>

namespace utils {
struct UsageTimes {
  unsigned busy;
  unsigned idle;
};

class CpuUsage {
public:
  enum ReadMode {
    Average,
    All,
  };
  CpuUsage(ReadMode mode = All);

  const UsageTimes &getAverageUsage() const {
    return averageUsage;
  }

  const UsageTimes &getCoreUsage(unsigned coreIdx) const {
    return coresUsage[coreIdx];
  }

private:
  UsageTimes averageUsage;
  std::vector<UsageTimes> coresUsage;
};

}