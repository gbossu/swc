#pragma once

#include "DataReaderBase.h"
#include "DataUsage.h"

namespace utils
{

class MemStatsReader : public DataReaderBase {
public:
  MemStatsReader();
  void update() override;
  float getDefaultValue() const override;
private:
  DataUsage physicalMem;
  unsigned unit = 1000; //1000 <==> kB
};

} // namespace utils
