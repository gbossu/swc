#include "DiskStatsReader.h"
#include "DataFetchError.h"
#include <sys/statvfs.h>

namespace utils
{

float DiskStatsReader::getDiskUsagePercent(const std::string &path) const
{
  struct statvfs64 diskStats;
  if (statvfs64(path.c_str(), &diskStats))
    throw DataFetchError("Could not read disk usage for " + path);
  
  auto usedBlocks = diskStats.f_blocks - diskStats.f_bfree;
  return usedBlocks * 100.f / diskStats.f_blocks;
}

} // namespace utils
