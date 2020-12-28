#include "CPUStatsReader.h"
#include "DataFetchError.h"
#include <fstream>
#include <array>
#include <thread>

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

  // Ignore remaining numbers
  input.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

  unsigned busyTime = times[USER] + times[NICED] + times[SYSTEM] +
                      times[IOWAIT] + times[IRQ] + times[SOFTIRQ];
  unsigned idleTime = times[IDLE];
  return UsageTimes{busyTime, idleTime};
}

CpuUsage::CpuUsage(ReadMode mode)
{
  if (mode == All) {
    unsigned numCores = std::thread::hardware_concurrency();
    if (numCores == 0)
      numCores = 1;
    coresUsage.resize(numCores);
    coresDiffUsage.resize(numCores);
  }

  try {
    update();
  } catch (const std::out_of_range &e) {
    throw DataFetchError(
        "Could not read the number of CPU cores properly");
  }
}

void CpuUsage::update() {
  std::ifstream ifs("/proc/stat");
  std::string label;
  for (;;) {
    ifs >> label;
    if (label.compare(0, 3, "cpu") != 0)
      break;
    if (label.size() == 3) {
      UsageTimes lastUsage = averageUsage;
      averageUsage = readUsage(ifs);
      averageDiffUsage = averageUsage - lastUsage;
      if (coresUsage.empty())
        break;
    } else {
      auto coreIdx = std::stoul(label.substr(3));
      UsageTimes lastUsage = coresUsage[coreIdx];
      coresUsage[coreIdx] = readUsage(ifs);
      coresDiffUsage[coreIdx] = coresUsage[coreIdx] - lastUsage;
    }
  }
}

float CpuUsage::getAverageBusyPercent() const
{
  auto totalTime = averageDiffUsage.busy + averageDiffUsage.idle;
  return averageDiffUsage.busy * 100.f / totalTime;
}

float CpuUsage::getCoreBusyPercent(unsigned coreIdx) const
{
  const UsageTimes &core = coresDiffUsage[coreIdx];
  auto totalTime = core.busy + core.idle;
  return core.busy * 100.f / totalTime;
}

}