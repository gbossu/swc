#include "AnimatedContainer.h"

extern "C" {
#include <xdo.h>
}

AnimatedContainer::AnimatedContainer(WId windowId, QWidget *p) :
    QWidget(p)
{
    embedWindow(windowId);
}

AnimatedContainer::AnimatedContainer(const QString &className, QWidget *p) :
    QWidget(p)
{
    Window *windowList = nullptr;
    unsigned int windowsNumber = 0;
    const char* windowPattern = className.toStdString().c_str();

    xdo_t * xdoInstance = xdo_new(nullptr);
    xdo_search_t searchReq;
    memset(&searchReq, 0, sizeof(xdo_search_t));
    searchReq.max_depth = -1;
    searchReq.require = xdo_search_t::SEARCH_ALL;
    searchReq.winclassname = windowPattern;
    searchReq.searchmask = SEARCH_CLASSNAME;
    searchReq.limit = 0;

    if (xdo_search_windows(xdoInstance, &searchReq, &windowList, &windowsNumber))
        qFatal("Couldn't find window");
    xdo_free(xdoInstance);

    if (windowsNumber == 1)
        embedWindow(WId(windowList[0]));
    else
        qFatal("Error: Found %u window(s)", windowsNumber);
}

void AnimatedContainer::embedWindow(WId windowId)
{
    auto size = QSize(360, 400);
    auto width = QApplication::desktop()->width();

    auto existingWindow = QWindow::fromWinId(windowId);
    QWidget *container = QWidget::createWindowContainer(existingWindow, this);
//    container->setFocusPolicy(Qt::TabFocus);
    container->resize(size);
    container->show();

    auto windowFlags = Qt::WindowFlags(Qt::BypassWindowManagerHint);
//    container->setWindowFlags(windowFlags);

    // Start a slide from the top of the screen
    QPropertyAnimation *slideIn = new QPropertyAnimation(this, "pos");
    slideIn->setEasingCurve(QEasingCurve(QEasingCurve::OutQuad));
    slideIn->setDuration(400);
    slideIn->setStartValue(QPoint(width / 2, -200));
    slideIn->setEndValue(QPoint(width / 2, 0));
    slideIn->start();
    connect(slideIn, SIGNAL(finished()), SLOT(slideInFinished()));
    connect(slideIn, SIGNAL(finished()), slideIn, SLOT(deleteLater()));
}

void AnimatedContainer::slideInFinished()
{
    // Wait some time
    QTimer::singleShot(3000, this, SLOT(pauseFinished()));
}

void AnimatedContainer::pauseFinished()
{
    // Do nothing yet
    //        connect(slideOut, SIGNAL(finished()), SLOT(slideOutFinished()));
    //        connect(slideOut, SIGNAL(finished()), slideOut, SLOT(deleteLater()));
}

void AnimatedContainer::slideOutFinished()
{
    // Do nothing yet
}
