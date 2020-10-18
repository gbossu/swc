#include "DiskStatsReader.h"
#include <sys/statvfs.h>

namespace utils
{

class DiskInfoError : public std::exception {
public:
  DiskInfoError(std::string message)
      : message(std::move(message)) {}
  const char *what() const noexcept override {
    return message.c_str();
  }
private:
  std::string message;
};

float DiskStatsReader::getDiskUsagePercent(const std::string &path) const
{
  struct statvfs64 diskStats;
  if (statvfs64(path.c_str(), &diskStats))
    throw DiskInfoError("Could not read disk usage for " + path);
  
  auto usedBlocks = diskStats.f_blocks - diskStats.f_bfree;
  return usedBlocks * 100.f / diskStats.f_blocks;
}

} // namespace utils
