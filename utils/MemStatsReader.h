#pragma once

#include "DataReaderBase.h"

namespace utils
{

struct MemUsage {
  unsigned long long total;
  unsigned long long available;
};

class MemStatsReader : public DataReaderBase {
public:
  MemStatsReader();
  void update() override;
  float getDefaultValue() const override;
private:
  MemUsage physicalMem;
  unsigned unit = 1000; //1000 <==> kB
};

} // namespace utils
