#include "CPUStatsReader.h"
#include <fstream>
#include <array>

namespace utils {

static UsageTimes readUsage(std::istream &input) {
  enum IdxNames {
    USER,
    NICED,
    SYSTEM,
    IDLE,
    IOWAIT,
    IRQ,
    SOFTIRQ
  };
  std::array<unsigned, 7> times;
  for (size_t idx = 0; idx < times.size(); ++idx)
    input >> times[idx];

  unsigned busyTime = times[USER] + times[NICED] + times[SYSTEM] +
                      times[IOWAIT] + times[IRQ] + times[SOFTIRQ];
  unsigned idleTime = times[IDLE];
  return UsageTimes{busyTime, idleTime};
}

CpuUsage::CpuUsage(ReadMode mode) {
  std::ifstream ifs("/proc/stat");
  std::string label;
  for (;;) {
    ifs >> label;
    if (label.compare(0, 3, "cpu") != 0)
      break;
    if (label.size() == 3) {
      averageUsage = readUsage(ifs);
      if (mode == Average)
        break;
    } else {
      auto coreIdx = std::stoul(label.substr(3));
      coresUsage[coreIdx] = readUsage(ifs);
    }
  }
}
}