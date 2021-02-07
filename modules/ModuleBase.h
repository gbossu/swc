#pragma once

#include <Qt>
#include <QLabel>

class QWidget;
class QGridLayout;

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
  virtual QWidget &getWidget() const = 0;

  /// Sets the title which will be displayed with the module.
  virtual void setTitle(const std::string &title) = 0;

  /// If it exists, returns the grid used to organize the sub-widgets of
  /// this Module. Otherwise creates it and sets getWidget() as its parent.
  /// The grid is used to e.g. add annotations.
  QGridLayout &getOrCreateInnerGrid();

private:
  QGridLayout *innerGrid = nullptr;
};

/// A label to annotate Modules, e.g. a title, axis value...
/// @param module The Module to annotate
/// @param align Text alignment
/// @param row Row number in the Module's inner grid
/// @param line Line number in the Module's inner grid
class ModuleAnnotation : public QLabel {
public:
  ModuleAnnotation(ModuleBase &module, Qt::Alignment align, int row, int col);
};

}