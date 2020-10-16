#include "MemStatsReader.h"
#include <fstream>
#include <string>
#include <limits>

namespace utils
{

class MemInfoError : public std::exception {
public:
  MemInfoError(std::string message)
      : message(std::move(message)) {}
  const char *what() const noexcept override {
    return message.c_str();
  }
private:
  std::string message;
};

static unsigned long long
convert(unsigned long long num, const std::string &fromUnit, unsigned toUnit)
{
  unsigned curUnit;
  if (fromUnit == "kB")
    curUnit = 1000;
  else if (fromUnit == "mB")
    curUnit = 1000000;
  else if (fromUnit == "gB")
    curUnit = 1000000000;
  else if (fromUnit == "B")
    curUnit = 1;
  else
    throw MemInfoError("Cannot read number unit.");

  if (toUnit <= curUnit)
    return num * (curUnit / toUnit);
  return num / (toUnit / curUnit);
}

MemStatsReader::MemStatsReader()
{
  update();
}

void MemStatsReader::update()
{
  std::ifstream input("/proc/meminfo");

  unsigned found = 0;
  std::string curWord;

  while (input >> curWord) {
    if (curWord == "MemTotal:") {
      input >> physicalMem.total;
      physicalMem.total.convert(unit);
      ++found;
    } else if (curWord == "MemAvailable:") {
      input >> physicalMem.available;
      physicalMem.available.convert(unit);
      ++found;
    } else {
      // Ignore irrelevant lines
      input.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      continue;
    }

    if (found == 2)
      return;
  }

  throw MemInfoError("Could not read memory usage.");
}

float MemStatsReader::getDefaultValue() const
{
  auto usedMem = *physicalMem.total - *physicalMem.available;
  return usedMem * 100.f / *physicalMem.total;
}

} // namespace utils
