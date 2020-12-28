#include "DataUsage.h"
#include "DataFetchError.h"

namespace utils {

unsigned long long DataQuantity::get(unsigned inUnit) const
{
  if (inUnit <= unit)
    return quantity * (unit / inUnit);
  return quantity / (inUnit / unit);
}

void DataQuantity::convert(unsigned toUnit)
{
  quantity = get(toUnit);
  unit = toUnit;
}

std::istream &operator>>(std::istream &in, DataQuantity &out)
{
  std::string unitAsStr;
  in >> out.quantity >> unitAsStr;

  if (unitAsStr == "kB")
    out.unit = 1000;
  else if (unitAsStr == "mB")
    out.unit = 1000000;
  else if (unitAsStr == "gB")
    out.unit = 1000000000;
  else if (unitAsStr == "B")
    out.unit = 1;
  else
    throw DataFetchError("Cannot read data unit.");
  return in;
}

}