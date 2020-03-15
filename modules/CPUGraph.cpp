#include "CPUGraph.h"
#include <CPUStatsReader.h>

namespace modules {

CpuGraph::CpuGraph(const QSize &size, QWidget *parent)
    : QObject(parent)
{
  series = new QtCharts::QLineSeries();
  // TODO: try to use OpenGL when feasible
  // series->setUseOpenGL(true);

  auto chart = new QtCharts::QChart();
  chart->legend()->hide();
  chart->addSeries(series);

  // TODO: try to use animations without high CPU usage.
  // chart->setAnimationDuration(500);
  // chart->setAnimationOptions(QtCharts::QChart::SeriesAnimations);

  xAxis = new QtCharts::QValueAxis();
  xAxis->setLabelsVisible(false);
  xAxis->setRange(0, numPoints - 1);
  chart->addAxis(xAxis, Qt::AlignBottom);
  series->attachAxis(xAxis);
  auto yAxis = new QtCharts::QValueAxis();
  yAxis->setLabelsVisible(false);
  yAxis->setRange(0, 100);
  chart->addAxis(yAxis, Qt::AlignLeft);
  series->attachAxis(yAxis);

  auto margins = QMargins(3, 3, 3, 3);
  chart->setPlotArea(QRectF(QPointF(3, 3), size.shrunkBy(margins)));
  chart->setBackgroundRoundness(3);

  chartView = new QtCharts::QChartView(chart, parent);
  chartView->setRenderHint(QPainter::Antialiasing);
  chartView->resize(size);
  chartView->show();

  connect(&timer, &QTimer::timeout, this, &CpuGraph::handleTimeout);
  timer.setTimerType(Qt::CoarseTimer);
  timer.setInterval(1000);
  timer.start();
}

void CpuGraph::handleTimeout()
{
  utils::CpuUsage newUsage;
  auto &oldAvg = lastUsage.getAverageUsage();
  auto &newAvg = newUsage.getAverageUsage();
  auto busyTime = newAvg.busy - oldAvg.busy;
  auto idleTime = newAvg.idle - oldAvg.idle;
  auto totalTime = busyTime + idleTime;

  // TODO: use totalTime to compute x index
  if (idx >= numPoints) {
    series->remove(0);
    auto newMin = qreal(idx - numPoints + 1);
    auto newMax = qreal(idx);
    xAxis->setRange(newMin, newMax);
  }

  // TODO: handle overflow of idx
  *series << QPointF(idx++, busyTime * 100.f / totalTime);
  lastUsage = newUsage;
}

} // namespace modules