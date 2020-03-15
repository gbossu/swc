#include "AnimatedContainer.h"
#include "Settings.h"
#include <QThread>
#include <signal.h>

AnimatedContainer::AnimatedContainer(std::unique_ptr<Settings> &&settings)
    : settings(std::move(settings))
{
  this->setWindowFlags(this->settings->getWindowFlags());
}

AnimatedContainer::~AnimatedContainer() {}

void AnimatedContainer::animate()
{
  // Request change of state
  emit needAnimate();
}

void AnimatedContainer::initSlideMachine()
{
  // Get the transition time
  const int transitionTime = settings->getInt("animation/duration");
  if (transitionTime < 0 || transitionTime > 10000) {
    qWarning("Error: animation duration should be between 0 and 10000");
    return;
  }

  // Create the slide machine with two states: hidden and visible
  slideMachine = new QStateMachine(this);
  hiddenState = new QState(slideMachine);
  hiddenState->assignProperty(this, "geometry", minimumGeometry);
  visibleState = new QState(slideMachine);
  visibleState->assignProperty(this, "geometry", maximumGeometry);
  slideMachine->setInitialState(hiddenState);

  // Create the transitions
  hiddenState->addTransition(this, SIGNAL(needAnimate()), visibleState);
  visibleState->addTransition(this, SIGNAL(needAnimate()), hiddenState);

  // Add a transition animation
  auto slide = new QPropertyAnimation(this, "geometry");
  slide->setEasingCurve(QEasingCurve(QEasingCurve::OutQuad));
  slide->setDuration(transitionTime);
  slideMachine->addDefaultAnimation(slide);
}

void AnimatedContainer::setupContainerWidget(const QSize &expandedSize) {
  // Get the position
  QPointF pos;
  const QString positionType = settings->getString("container/position_type");
  if (positionType == "absolute")
    pos = settings->getPoint("container/position");
  else if (positionType == "percent") {
    auto desktopSize = QApplication::desktop()->size();
    auto percents = settings->getPoint("container/position");
    pos.setX(desktopSize.width() * (percents.x() / 100.));
    pos.setY(desktopSize.height() * (percents.y() / 100.));
  } else {
    qWarning("Error: position_type setting not recognized");
    return;
  }

  // Get the slide direction to define the anchor and the geometries
  const QString direction = settings->getString("animation/direction");
  if (direction == "down") {
    QPoint anchor = QPoint(expandedSize.width() / 2, 0);
    minimumGeometry = QRect(pos.toPoint() - anchor,
                            QSize(expandedSize.width(), 0));
    maximumGeometry = QRect(pos.toPoint() - anchor,
                            expandedSize);
  } else if (direction == "up") {
    QPoint anchor = QPoint(expandedSize.width() / 2, 0);
    minimumGeometry = QRect(pos.toPoint() - anchor,
                            QSize(expandedSize.width(), 0));
    anchor.setY(expandedSize.height());
    maximumGeometry = QRect(pos.toPoint() - anchor,
                            expandedSize);
  } else if (direction == "right") {
    QPoint anchor = QPoint(0, expandedSize.height() / 2);
    minimumGeometry = QRect(pos.toPoint() - anchor,
                            QSize(0, expandedSize.height()));
    maximumGeometry = QRect(pos.toPoint() - anchor,
                            expandedSize);
  } else if (direction == "left") {
    QPoint anchor = QPoint(0, expandedSize.height() / 2);
    minimumGeometry = QRect(pos.toPoint() - anchor,
                            QSize(0, expandedSize.height()));
    anchor.setX(expandedSize.width());
    maximumGeometry = QRect(pos.toPoint() - anchor,
                            expandedSize);
  } else {
    qWarning("Error: direction setting not recognized");
    return;
  }

  // Place the animated container
  // By default it will be hidden, it will "slide in" after a call to animate
  this->setGeometry(minimumGeometry);
  this->setMinimumSize(minimumGeometry.size());
  this->setMaximumSize(maximumGeometry.size());

  // Initialize and start the state machine
  initSlideMachine();
  if (slideMachine)
    slideMachine->start();
}
