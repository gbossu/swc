#pragma once

#include <QtCore/QSize>
#include <QtCore/QPointer>
#include <QtCore/QTimer>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include "ModuleUtils.h"

namespace modules {

class LineGraph : public ModuleBase {
public:
  LineGraph(const QSize &size, QWidget *parent, size_t numPoints);
  void add(float value, unsigned index) override;

private:
  QtCharts::QLineSeries *series;
  QtCharts::QValueAxis *xAxis;
  QtCharts::QChartView *chartView;
  int idx = 0;
  size_t numPoints;
};

} // namespace modules