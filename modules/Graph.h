#pragma once

#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QBarSet>
#include "ModuleUtils.h"
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
  QWidget *getWidget() const override;
  void setTitle(const std::string &title) override;

protected:
  QtCharts::QChart *chart;
  QtCharts::QChartView *chartView;
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