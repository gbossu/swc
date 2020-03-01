#pragma once

#include <QtGui>
#include <QtWidgets>

class Settings;

class AnimatedContainer: public QWidget
{
    Q_OBJECT
public:
    AnimatedContainer(const Settings &settings, QWidget *p = nullptr);
    virtual ~AnimatedContainer() override;

    // TODO: use exceptions instead
    bool hasWidget() const;

    /// Called from DBus to request an animation for the container, and toggle
    /// its state (e.g. from hidden to visible).
    virtual void animate() = 0;

protected:
    void initSlideMachine();

    QPointer<QWidget> container;
    QPointer<QStateMachine> slideMachine;
    QState *hiddenState = nullptr;
    QState *visibleState = nullptr;
    const Settings &settings;
    QRect minimumGeometry;
    QRect maximumGeometry;

signals:
    void needAnimate();
};