#pragma once

#include <string>
#include <memory>
#include <map>
#include <limits>
#include <variant>
#include <json.hpp>

namespace modules {

using miliseconds = unsigned;

class ModuleGridError : public std::exception {
public:
  ModuleGridError(const std::string &details) :
      msg("ModuleGrid error: " + details) {}
  const char *what() const noexcept override {
    return msg.c_str();
  }
private:
  std::string msg;
};

/// A namespace containing all allowed types of schemas.
namespace schemas {
  struct Line {
    unsigned points;
    unsigned lineWidth;
  };

  struct Bar {
  };
}
using SchemaVariant = std::variant<schemas::Line, schemas::Bar>;

/// Represents the information gathered from a JSON about a ModuleSchema.
class ModuleSchema {
public:
  ModuleSchema(const nlohmann::json &jsonSchema);

  const std::string &getName() const {
    return name;
  }
  const SchemaVariant &getVariant() const {
    return variant;
  }

private:
  std::string name;
  SchemaVariant variant;
};

namespace dataSources
{
struct Cpu {
  std::vector<unsigned> cores;
};
struct Mem {
  bool physicalMem = true;
  bool virtualMem = false;
  bool swap = false;
};
struct Disk {
  std::string path;
};
} // namespace dataSources
using DataSourceVariant =
    std::variant<dataSources::Cpu, dataSources::Mem, dataSources::Disk>;

/// Represents the information gathered from a JSON about a Module.
/// The goal is to parse the JSON and make sure it is valid, so the ModuleGrid
/// has no more error-checking to do.
class ModuleInfo {
public:
  ModuleInfo(const nlohmann::json &jsonModule);
  const DataSourceVariant &getDataSource() const {
    return dataSource;
  }
  const std::string &getSchemaName() const {
    return schemaName;
  }
  miliseconds getRefreshDelay() const {
    return refreshDelay;
  }
  unsigned getRow() const {
    return row;
  }
  unsigned getColumn() const {
    return column;
  }
  const std::string &getTitle() const {
    return title;
  }
private:
  DataSourceVariant dataSource;
  std::string schemaName;
  miliseconds refreshDelay;
  unsigned row;
  unsigned column;
  std::string title;
};

/// Represents the information gathered from a JSON about a ModuleGrid.
class ModuleGridInfo {
public:
  using ConstModuleIt = std::vector<ModuleInfo>::const_iterator;

  ModuleGridInfo(const nlohmann::json &jsonGrid);

  /// Returns the biggest row number among all Modules.
  unsigned getMaxRow() const {
    return maxRow;
  }

  /// @return the schema corresponding to the given name.
  /// throws @ref ModuleGridError if it cannot be found.
  const ModuleSchema &getSchema(const std::string &schemaName) const;

  /// @return Get the specified stretch factors of columns.
  const std::vector<int> &getColumnStretchFactors() const {
    return columnWidths;
  }

  ConstModuleIt begin() const;
  ConstModuleIt end() const;

private:
  unsigned maxRow = std::numeric_limits<unsigned>::min();
  std::map<std::string, ModuleSchema> moduleSchemas;
  std::vector<ModuleInfo> modules;
  std::vector<int> columnWidths;
};

}