#include "LineGraph.h"
#include <QGraphicsLayout>

namespace modules {

/// A standard QChart with the exception that the plot area always occupies
/// the whole available space.
class FullSizeChart : public QtCharts::QChart {
protected:
  void resizeEvent(QGraphicsSceneResizeEvent *event) override {
    QtCharts::QChart::resizeEvent(event);
    setPlotArea(geometry());
    // Force the "grid" to be re-drawn
    for (QtCharts::QAbstractAxis *ax : axes()) {
      ax->hide();
      ax->show();
    }
  }
};

LineGraph::LineGraph(const ModuleSize &modSize, size_t numPoints)
    : numPoints(numPoints)
{
  series = new QtCharts::QLineSeries();
  // TODO: try to use OpenGL when feasible
  // series->setUseOpenGL(true);

  auto chart = new FullSizeChart();
  chartView = new QtCharts::QChartView(chart);
  chartView->setRenderHint(QPainter::Antialiasing);

  // Note: it seems that setting null margins does not really help,
  // only setting the PlotArea to occupy the whole chart works.
  chart->setMargins(QMargins(0, 0, 0, 0));
  chart->setContentsMargins(QMargins(0, 0, 0, 0));
  chart->layout()->setContentsMargins(0, 0, 0, 0);
  chartView->setContentsMargins(QMargins(0, 0, 0, 0));

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
}

LineGraph::~LineGraph()
{
  delete chartView;
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

QWidget *LineGraph::getWidget() const
{
  return chartView;
}

} // namespace modules