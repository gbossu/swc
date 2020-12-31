#pragma once

#include "DataReaderBase.h"
#include <string>
#include <cstdint>
#include <fstream>

namespace utils {
struct InterfaceStats {
  uint64_t receivedBytes = 0U;
  uint64_t sentBytes = 0U;

  InterfaceStats operator-(const InterfaceStats &rhs) const {
    return InterfaceStats{receivedBytes - rhs.receivedBytes,
                          sentBytes - rhs.sentBytes};
  }
};

/// A simple class to extract the received/sent bytes for a specific network
/// interface. It uses the values from /sys/class/net/if_name/statistics/.
class NetworkStatsReader : public DataReaderBase {
public:
  NetworkStatsReader(const std::string &interfaceName);

  void update() override;

  /// Returns the sent/received data quantities by the interface between
  /// the last two calls to update.
  const InterfaceStats &getCurrentInterfaceStats() const {
    return diffStats;
  }

  float getDefaultValue() const override {
    return float(diffStats.receivedBytes);
  }

private:
  std::ifstream receivedBytesFD;
  std::ifstream sentBytesFD;
  InterfaceStats totalStats;
  InterfaceStats diffStats;
};

}