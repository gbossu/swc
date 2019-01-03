#ifndef SELFANIMATOR_H
#define SELFANIMATOR_H

#include <QtGui>
#include <QtWidgets>

extern "C" {
#include <xdo.h>
}

class Settings;

class AnimatedContainer: public QWidget
{
    Q_OBJECT
public:
    AnimatedContainer(Settings *settings, WId windowId, QWidget *p = nullptr);
    AnimatedContainer(Settings *settings, QString const& className, QWidget *p = nullptr);
    AnimatedContainer(Settings *settings, int pid, QWidget *p = nullptr);
    AnimatedContainer(Settings *settings, int pid, QString const& className, QWidget *p = nullptr);
    virtual ~AnimatedContainer() override;
    void releaseWindow();
    bool hasWindow() const;

public slots:
    void animate();

private:
    void embedWindow(WId windowId);
    void initSlideMachine();
    xdo_search_t createSearchRequest();
    WId searchWindow(xdo_search_t const& searchReq, int maxTries);
    QSize getWindowSize(WId windowId) const;

    QPointer<QWindow> existingWindow;
    QPointer<QWidget> container;
    QPointer<QStateMachine> slideMachine;
    xdo_t * xdoInstance;
    Settings *settings;
    QSize originalSize;

private slots:
    void slideFinished();
    void pauseFinished();
    void closeEvent(QCloseEvent *event) override;

signals:
    void needAnimate();
};

#endif // SELFANIMATOR_H
