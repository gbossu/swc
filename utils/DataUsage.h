#pragma once

#include <istream>

namespace utils
{

/// Represents a number in a specific unit (e.g. kB)
class DataQuantity {
public:
  /// Creates a null quantity
  DataQuantity() = default;

  /// Represents a number in the form quantity*unit.
  DataQuantity(unsigned long long quantity, unsigned unit)
    : quantity(quantity), unit(unit) {}

  /// Returns a quantity so that:
  ///   quantity * unit == returnedQuantity * inUnit
  unsigned long long get(unsigned inUnit) const;

  /// Change quantity so that:
  ///   oldQuantity * oldUnit == newQuantity * toUnit
  void convert(unsigned toUnit);

  /// Returns the saved quantity without conversion.
  unsigned long long operator*() const {
    return quantity;
  }

  /// Update the attributes using in >> quantity >> unitAsString;
  friend std::istream &operator>>(std::istream &in, DataQuantity &out);

private:
  unsigned long long quantity = 0;
  unsigned unit = 1000; //1000 <==> kB
};

struct DataUsage {
  DataQuantity total;
  DataQuantity available;
};

} // namespace utils
