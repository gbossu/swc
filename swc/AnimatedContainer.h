#pragma once

#include <QtGui>
#include <QtWidgets>
#include <memory>

class Settings;

class AnimatedContainer: public QWidget
{
    Q_OBJECT
public:
    AnimatedContainer(std::unique_ptr<Settings> &&settings);
    virtual ~AnimatedContainer() override;

    /// Called from DBus to request an animation for the container, and toggle
    /// its state (e.g. from hidden to visible).
    virtual void animate();

protected:

    void changeEvent(QEvent *event) override;

    void initSlideMachine();

    /// Configures the container so it can be animated.
    /// @param expandedSize the size of the container when it is expanded.
    void setupContainerWidget(const QSize &expandedSize);

    QPointer<QStateMachine> slideMachine;
    QState *hiddenState = nullptr;
    QState *visibleState = nullptr;
    std::unique_ptr<Settings> settings;
    QRect minimumGeometry;
    QRect maximumGeometry;

signals:
    void needAnimate();
    void gotUnfocused();
};