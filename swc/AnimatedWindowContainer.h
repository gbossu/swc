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
    AnimatedWindowContainer(std::unique_ptr<Settings> &&settings, WId windowId,
                            QWidget *p = nullptr);
    AnimatedWindowContainer(std::unique_ptr<Settings> &&settings,
                            QString const& className, QWidget *p = nullptr);
    AnimatedWindowContainer(std::unique_ptr<Settings> &&settings,
                            std::unique_ptr<QProcess> &&process,
                            QWidget *p = nullptr);
    AnimatedWindowContainer(std::unique_ptr<Settings> &&settings,
                            std::unique_ptr<QProcess> &&process,
                            QString const& className, QWidget *p = nullptr);
    virtual ~AnimatedWindowContainer() override;

public slots:
    void animate() override;

protected:
    void initSlideMachine();
    void embedWindow(WId windowId);
    void releaseWindow();
    WId searchWindow(xdo_search_t const& searchReq, int maxTries);
    QSize getWindowSize(WId windowId) const;

    QPointer<QWindow> existingWindow;
    xdo_t * xdoInstance;
    std::unique_ptr<QProcess> executable;

private slots:
    void containerShown();
    void containerHidden();
    void closeEvent(QCloseEvent *event) override;
};
