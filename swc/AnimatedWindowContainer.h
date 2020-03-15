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
    AnimatedWindowContainer(std::unique_ptr<Settings> &&settings, WId windowId);
    AnimatedWindowContainer(std::unique_ptr<Settings> &&settings,
                            QString const& className);
    AnimatedWindowContainer(std::unique_ptr<Settings> &&settings,
                            std::unique_ptr<QProcess> &&process);
    AnimatedWindowContainer(std::unique_ptr<Settings> &&settings,
                            std::unique_ptr<QProcess> &&process,
                            QString const& className);
    virtual ~AnimatedWindowContainer() override;


    // TODO: use exceptions instead
    bool hasWidget() const {
        return container != nullptr;
    }

public slots:
    void animate() override;

protected:
    void setupWindowContainer(WId windowId);
    void releaseWindow();
    WId searchWindow(xdo_search_t const& searchReq, int maxTries);
    QSize getWindowSize(WId windowId) const;

    QPointer<QWindow> existingWindow;
    QPointer<QWidget> container;
    xdo_t * xdoInstance;
    std::unique_ptr<QProcess> executable;

private slots:
    void containerShown();
    void containerHidden();
    void closeEvent(QCloseEvent *event) override;
};
