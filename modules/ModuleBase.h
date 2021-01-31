#pragma once

class QWidget;

namespace utils {
class DataReaderBase;
}

namespace modules {

/// Base class for all Modules.
class ModuleBase {
public:
  virtual ~ModuleBase() = default;

  /// Used by DataForwarders to communicate values from DataReaders to Modules.
  virtual void add(float value, unsigned index = 0U) = 0;

  /// Called when a specific index of that Module is connected to a DataReader
  /// via a DataForwarder.
  virtual void registerDataProvider(const utils::DataReaderBase &provider,
                                    unsigned index = 0U) = 0;

  /// Returns the Qt widget representing the module.
  virtual QWidget *getWidget() const = 0;

  /// Sets the title which will be displayed with the module.
  virtual void setTitle(const std::string &title) = 0;
};

}