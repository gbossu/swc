#pragma once

#include "AnimatedContainer.h"
#include <QtGui>
#include <QtWidgets>

extern "C" {
#include <xdo.h>
}

class AnimatedWindowContainer: public AnimatedContainer
{
    Q_OBJECT
public:
    AnimatedWindowContainer(const Settings &settings, WId windowId,
                            QWidget *p = nullptr);
    AnimatedWindowContainer(const Settings &settings, QString const& className,                         QWidget *p = nullptr);
    AnimatedWindowContainer(const Settings &settings, int pid,
                            QWidget *p = nullptr);
    AnimatedWindowContainer(const Settings &settings, int pid,
                            QString const& className, QWidget *p = nullptr);
    virtual ~AnimatedWindowContainer() override;
    void setExecutalbe(QProcess *exec);

public slots:
    void animate() override;

protected:
    void initSlideMachine();
    void embedWindow(WId windowId);
    void releaseWindow();
    xdo_search_t createSearchRequest();
    WId searchWindow(xdo_search_t const& searchReq, int maxTries);
    QSize getWindowSize(WId windowId) const;

    QPointer<QWindow> existingWindow;
    xdo_t * xdoInstance;
    QProcess *executable;

private slots:
    void containerShown();
    void containerHidden();
    void closeEvent(QCloseEvent *event) override;
};
