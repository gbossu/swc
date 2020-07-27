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
}
using SchemaVariant = std::variant<schemas::Line>;

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

/// Represents the information gathered from a JSON about a Module.
class ModuleInfo {
public:
  ModuleInfo(const nlohmann::json &jsonModule);
  const std::string &getSourceName() const {
    return sourceName;
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
private:
  std::string sourceName;
  std::string schemaName;
  miliseconds refreshDelay;
  unsigned row;
  unsigned column;
};

/// Represents the information gathered from a JSON about a ModuleGrid.
class ModuleGridInfo {
public:
  using ConstModuleIt = std::vector<ModuleInfo>::const_iterator;

  ModuleGridInfo(const nlohmann::json &jsonGrid);

  /// Returns the biggest line number among all Modules.
  unsigned getMaxLine() const {
    return maxLine;
  }

  /// @return the schema corresponding to the given name.
  /// throws @ref ModuleGridError if it cannot be found.
  const ModuleSchema &getSchema(const std::string &schemaName) const;

  ConstModuleIt begin() const;
  ConstModuleIt end() const;

private:
  unsigned maxLine = std::numeric_limits<unsigned>::max();
  std::map<std::string, ModuleSchema> moduleSchemas;
  std::vector<ModuleInfo> modules;
};

}