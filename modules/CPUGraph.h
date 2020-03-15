#pragma once

#include <QtCore/QSize>
#include <QtCore/QPointer>
#include <QtCore/QTimer>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <CPUStatsReader.h>

namespace modules {

class CpuGraph : public QObject {
  Q_OBJECT
public:
  CpuGraph(const QSize &size, QWidget *parent);

public slots:
  void handleTimeout();

private:
  QtCharts::QLineSeries *series;
  QtCharts::QValueAxis *xAxis;
  QtCharts::QChartView *chartView;
  QTimer timer;
  int idx = 0;
  size_t numPoints = 20;
  utils::CpuUsage lastUsage;
};

} // namespace modules