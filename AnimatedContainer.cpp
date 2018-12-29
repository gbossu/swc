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

AnimatedContainer::~AnimatedContainer()
{
    QApplication::exit();
}

void AnimatedContainer::releaseWindow()
{
    existingWindow->setParent(nullptr);
    existingWindow->setFramePosition(this->pos());
    container->setParent(nullptr);

    existingWindow = nullptr;
}

void AnimatedContainer::embedWindow(WId windowId)
{
    originalSize = QSize(360, 400);
    auto size = QSize(originalSize.width(), 0);
    auto width = QApplication::desktop()->width() - size.width();
    auto pos = QPoint(width / 2, 0);

    // Place the animated container
    this->setMinimumSize(size);
    this->setMaximumSize(originalSize);
    this->move(pos);

    // Embed the window using the windowId
    existingWindow = QWindow::fromWinId(windowId);
    container = QWidget::createWindowContainer(existingWindow, this);
    container->resize(originalSize);

    // Start a slide from the top of the screen
    QPropertyAnimation *slideIn = new QPropertyAnimation(this, "size");
    slideIn->setEasingCurve(QEasingCurve(QEasingCurve::OutQuad));
    slideIn->setDuration(400);
    slideIn->setStartValue(size);
    slideIn->setEndValue(originalSize);
    slideIn->start();
    connect(slideIn, SIGNAL(finished()), SLOT(slideInFinished()));

    // Delete the container when it is closed
    this->setAttribute(Qt::WA_DeleteOnClose);
}

void AnimatedContainer::slideInFinished()
{
    // Wait some time
    QTimer::singleShot(1000, this, SLOT(pauseFinished()));
}

void AnimatedContainer::pauseFinished()
{
    // Do nothing yet
}

void AnimatedContainer::slideOutFinished()
{
    // Do nothing yet
}

void AnimatedContainer::closeEvent(QCloseEvent *event)
{
    releaseWindow();
    event->accept();
}
