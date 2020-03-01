#include "AnimatedContainer.h"
#include "Settings.h"
#include <QThread>
#include <signal.h>

AnimatedContainer::AnimatedContainer(
    std::unique_ptr<Settings> &&settings, QWidget *p) :
  QWidget(p), settings(std::move(settings))
{
  this->setWindowFlags(this->settings->getWindowFlags());
}

AnimatedContainer::~AnimatedContainer() {}

bool AnimatedContainer::hasWidget() const
{
    return container != nullptr;
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
    slideMachine = new QStateMachine;
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
