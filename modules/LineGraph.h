#pragma once

#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include "ModuleUtils.h"
#include "ModuleSize.h"

namespace modules {

class LineGraph : public ModuleBase {
public:
  LineGraph(const ModuleSize &size, QWidget *parent, size_t numPoints);
  void add(float value, unsigned index) override;

private:
  QtCharts::QLineSeries *series;
  QtCharts::QValueAxis *xAxis;
  QtCharts::QChartView *chartView;
  int idx = 0;
  size_t numPoints;
};

} // namespace modules