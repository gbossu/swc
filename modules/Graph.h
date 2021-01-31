#pragma once

#include <optional>
#include <limits>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QBarSet>
#include "ModuleBase.h"
#include "ModuleSize.h"

namespace modules {

/// Base class for all graphs, it will in particular setup the chart
/// to occupy all the available space.
class GraphBase : public ModuleBase {
public:
  /// Constructor
  /// @param size User-defined size requirements (if any).
  GraphBase(const ModuleSize &size);
  ~GraphBase();
  void add(float value, unsigned index) override;
  void registerDataProvider(const utils::DataReaderBase &provider,
                            unsigned index) override;
  QWidget *getWidget() const override;
  void setTitle(const std::string &title) override;

protected:
  QtCharts::QChart *chart;
  QtCharts::QChartView *chartView;
  QtCharts::QValueAxis *yAxis;
  std::optional<float> knownMaxValue;
  float maxMetValue = std::numeric_limits<float>::min();
};

class LineGraph : public GraphBase {
public:
  LineGraph(const ModuleSize &size, size_t numPoints);
  void add(float value, unsigned index) override;

private:
  QtCharts::QLineSeries *series;
  QtCharts::QValueAxis *xAxis;
  int idx = 0;
  size_t numPoints;
};

class BarGraph : public GraphBase {
public:
  BarGraph(const ModuleSize &size);
  void add(float value, unsigned index) override;
private:
  QtCharts::QBarSet *bar;
};

} // namespace modules