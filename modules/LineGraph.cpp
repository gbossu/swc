#include "LineGraph.h"

namespace modules {

LineGraph::LineGraph(const QSize &size, QWidget *parent)
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
}

void LineGraph::add(float value, unsigned index)
{
  assert(index == 0);

  // TODO: use totalTime to compute x index
  if (idx >= numPoints) {
    series->remove(0);
    auto newMin = qreal(idx - numPoints + 1);
    auto newMax = qreal(idx);
    xAxis->setRange(newMin, newMax);
  }

  // TODO: handle overflow of idx
  *series << QPointF(idx++, value);
}

} // namespace modules