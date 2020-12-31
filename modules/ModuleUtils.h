#pragma once

#include <QtCore/QTimer>
#include <functional>
#include <vector>
#include <memory>

class QWidget;

namespace utils {
class DataReaderBase;
}

namespace modules {

using miliseconds = unsigned;

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

/// Base class for all DataForwarders.
class DataForwarderBase {
public:
  DataForwarderBase(miliseconds interval);
protected:
  miliseconds interval;
  QTimer timer;
};

template<class DataFetcher, typename... Args>
class DataForwarder : public DataForwarderBase {
public:
  using timeoutAction = std::function<void(const DataFetcher &, ModuleBase &)>;

  /// Constructor
  /// @param interval Time between consecutive runs of the actions defined for
  ///                 this DataForwarder.
  DataForwarder(miliseconds interval, Args&&... args);

  /// Attach a module to this DataForwarder. This means that every time fresh
  /// data is fetched, the action specified by @ref callback will be performed.
  void addModule(ModuleBase &module, timeoutAction callback);

  /// Similar to @ref addModule, but automatically creates the action.
  /// Data from the DataFetcher will be read using
  /// DataReaderBase::getDefaultValue() and then passed to @ref module using
  /// @ref ModuleBase::add.
  void addModuleWithDefaultAction(ModuleBase &module);

  /// Regularly called (see the interval defined in the constructor) to perform
  /// the defined actions for every attached Module.
  void handleTimeout();

private:
  DataFetcher dataReader;
  std::vector<timeoutAction> actions;
  std::vector<ModuleBase *> modules;
};

template<class DataFetcher, typename... Args>
DataForwarder<DataFetcher, Args...>::DataForwarder(miliseconds interval, Args&&... args)
    : DataForwarderBase(interval), dataReader(std::forward<Args>(args)...)
{
  QObject::connect(&timer, &QTimer::timeout, [this]() {
    handleTimeout();
  });
  timer.start();
}

template<class DataFetcher, typename... Args>
void DataForwarder<DataFetcher, Args...>::handleTimeout()
{
  dataReader.update();

  // Run the corresponding action for each module.
  auto actionIt = actions.begin();
  for (ModuleBase *module : modules) {
    (*actionIt)(dataReader, *module);
    ++actionIt;
  }
}

template<class DataFetcher, typename... Args>
void DataForwarder<DataFetcher, Args...>::addModule(ModuleBase &module,
                                           timeoutAction callback)
{
  module.registerDataProvider(dataReader);
  modules.push_back(&module);
  actions.push_back(callback);
}

template<class DataFetcher, typename... Args>
void DataForwarder<DataFetcher, Args...>::addModuleWithDefaultAction(
    ModuleBase &module)
{
  addModule(module, [](const DataFetcher &data, ModuleBase &module) {
    module.add(data.getDefaultValue());
  });
}

template<class DataFetcher, typename... Args>
std::unique_ptr<DataForwarder<DataFetcher, Args...>>
make_forwarder(miliseconds interval, Args&&... args)
{
  return std::make_unique<DataForwarder<DataFetcher, Args...>>(
      interval, std::forward<Args>(args)...);
}

}