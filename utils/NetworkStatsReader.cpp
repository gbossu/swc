#include "NetworkStatsReader.h"
#include "DataFetchError.h"
#include <fstream>

namespace utils {

NetworkStatsReader::NetworkStatsReader(const std::string &interfaceName)
{
  std::string interfaceStatsPath =
      "/sys/class/net/" + interfaceName + "/statistics/";
  receivedBytesFD.open(interfaceStatsPath + "rx_bytes");
  sentBytesFD.open(interfaceStatsPath + "tx_bytes");
  if (!receivedBytesFD.is_open() || !sentBytesFD.is_open()) {
    // TODO: use a warning instead?
    throw DataFetchError(
      "Could not read interface statistics for " + interfaceName);
  }

  update();
}

static void readNumber(std::ifstream &ifs, uint64_t &num)
{
  ifs.seekg(0);
  ifs.sync();
  ifs >> num;
}

void NetworkStatsReader::update()
{
  InterfaceStats lastStats = totalStats;
  readNumber(receivedBytesFD, totalStats.receivedBytes);
  readNumber(sentBytesFD, totalStats.sentBytes);
  diffStats = totalStats - lastStats;
}

}