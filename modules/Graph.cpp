#include "Graph.h"
#include "DataReaderBase.h"
#include <QGraphicsLayout>
#include <QtCharts/QBarSeries>

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

GraphBase::GraphBase(const ModuleSize &modSize)
{
  chart = new FullSizeChart();
  chartView = new QtCharts::QChartView(chart);
  chartView->setRenderHint(QPainter::Antialiasing);

  // Note: it seems that setting null margins does not really help,
  // only setting the PlotArea to occupy the whole chart works.
  chart->setMargins(QMargins(0, 0, 0, 0));
  chart->layout()->setContentsMargins(0, 0, 0, 0);
  chart->legend()->hide();

  // TODO: try to use animations without high CPU usage.
  // chart->setAnimationDuration(500);
  // chart->setAnimationOptions(QtCharts::QChart::SeriesAnimations);

  yAxis = new QtCharts::QValueAxis();
  yAxis->setLabelsVisible(false);
  chart->addAxis(yAxis, Qt::AlignLeft);

  maxLabel = new ModuleAnnotation(*this, Qt::AlignTop | Qt::AlignRight, 0, 1);
}

GraphBase::~GraphBase()
{
  delete chartView;
}

QWidget &GraphBase::getWidget() const
{
  return *chartView;
}

void GraphBase::add(float value, unsigned index)
{
  assert(index == 0);

  if (!knownMaxValue && value > maxMetValue) {
    maxMetValue = value;
    yAxis->setRange(0, value);
    yAxis->applyNiceNumbers();
    maxLabel->setText(QString::number(yAxis->max()));
  }
}

void GraphBase::registerDataProvider(const utils::DataReaderBase &provider, unsigned index)
{
  // TODO: refactor to registerDataInfo(const DataInfo &, unsigned index)
  // and provide data range if known, and unit (B, %, etc).
  assert(index == 0);
  knownMaxValue = provider.getMaxValue();
  if (knownMaxValue.has_value()) {
    assert(yAxis);
    yAxis->setRange(0, *knownMaxValue);
    maxLabel->setText(QString::number(*knownMaxValue));
  }
}

void GraphBase::setTitle(const std::string &title) {
  auto *titleAnot =
      new ModuleAnnotation(*this, Qt::AlignLeft | Qt::AlignTop, 0, 0);
  titleAnot->setText(QString::fromStdString(title));
}

LineGraph::LineGraph(const ModuleSize &modSize, size_t numPoints)
    : GraphBase(modSize), numPoints(numPoints)
{
  series = new QtCharts::QLineSeries();
  chart->addSeries(series);
  // TODO: try to use OpenGL when feasible
  // series->setUseOpenGL(true);

  xAxis = new QtCharts::QValueAxis();
  xAxis->setLabelsVisible(false);
  xAxis->setRange(0, numPoints - 1);
  chart->addAxis(xAxis, Qt::AlignBottom);
  series->attachAxis(xAxis);
  series->attachAxis(yAxis);
}

void LineGraph::add(float value, unsigned index)
{
  GraphBase::add(value, index);
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

BarGraph::BarGraph(const ModuleSize &size)
    : GraphBase(size)
{
  auto series = new QtCharts::QBarSeries();
  series->setBarWidth(1);

  bar = new QtCharts::QBarSet("");
  bar->insert(0, 0);
  series->append(bar);
  chart->addSeries(series);
  series->attachAxis(yAxis);
}

void BarGraph::add(float value, unsigned index)
{
  GraphBase::add(value, index);
  assert(index == 0); // Do not handle multiple bars so far.
  bar->replace(0, value);
}

} // namespace modules