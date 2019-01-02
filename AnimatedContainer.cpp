#include "AnimatedContainer.h"
#include "Settings.h"
#include <QThread>

AnimatedContainer::AnimatedContainer(Settings *settings, WId windowId, QWidget *p) :
    QWidget(p), settings(settings)
{
    embedWindow(windowId);
}

AnimatedContainer::AnimatedContainer(Settings *settings, const QString &className, QWidget *p) :
    QWidget(p), settings(settings)
{
    const char* windowPattern = className.toStdString().c_str();
    xdo_search_t searchReq = createSearchRequest();

    searchReq.winclassname = windowPattern;
    searchReq.searchmask = SEARCH_CLASSNAME;
    embedWindow(searchWindow(searchReq, settings->getInt("lookup/max_tries")));
}

AnimatedContainer::AnimatedContainer(Settings *settings, int pid, QWidget *p) :
    QWidget(p), settings(settings)
{
    xdo_search_t searchReq = createSearchRequest();

    searchReq.pid = pid;
    searchReq.searchmask = SEARCH_PID;
    embedWindow(searchWindow(searchReq, settings->getInt("lookup/max_tries")));
}

AnimatedContainer::AnimatedContainer(Settings *settings, int pid, QString const& className, QWidget *p) :
    QWidget(p), settings(settings)
{
    const char* windowPattern = className.toStdString().c_str();
    xdo_search_t searchReq = createSearchRequest();

    searchReq.pid = pid;
    searchReq.winclassname = windowPattern;
    searchReq.searchmask = SEARCH_PID | SEARCH_CLASSNAME;
    embedWindow(searchWindow(searchReq, settings->getInt("lookup/max_tries")));
}

AnimatedContainer::~AnimatedContainer()
{
}

void AnimatedContainer::releaseWindow()
{
    existingWindow->setParent(nullptr);
    existingWindow->setFramePosition(this->pos());
    container->setParent(nullptr);

    existingWindow = nullptr;
}

void AnimatedContainer::animate()
{
    // Resize the window to its original size
    // It is needed to avoid artifacts happening when the container is
    // created before the window is rendered
    existingWindow->resize(originalSize);

    // Start a slide from the top of the screen
    QPropertyAnimation *slideIn = new QPropertyAnimation(this, "size");
    slideIn->setEasingCurve(QEasingCurve(QEasingCurve::OutQuad));
    slideIn->setDuration(400);
    slideIn->setStartValue(size());
    slideIn->setEndValue(reverseSlide ? minimumSize() : maximumSize());
    reverseSlide = !reverseSlide;
    slideIn->start();

    // Call slideInFinished when the animation has finished
    connect(slideIn, SIGNAL(finished()), SLOT(slideInFinished()));
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

    // By default when embedding, the window will be hidden
    // It will "slide" after a call to animate
    this->resize(size);
    reverseSlide = false;
}

xdo_search_t AnimatedContainer::createSearchRequest()
{
    xdo_search_t searchReq;
    memset(&searchReq, 0, sizeof(xdo_search_t));
    searchReq.max_depth = -1;
    searchReq.require = xdo_search_t::SEARCH_ANY;
    searchReq.limit = 0;
    return searchReq;
}

WId AnimatedContainer::searchWindow(const xdo_search_t &searchReq, int maxTries)
{
    Window *windowList = nullptr;
    unsigned int windowsNumber = 0;

    // Get the sleep time from settings
    const int tryInterval = settings->getInt("lookup/try_interval");
    if (tryInterval < 0)
        qFatal("Error: negative try_interval time in config file");
    const unsigned int sleepTime = static_cast<unsigned int>(tryInterval);

    xdo_t * xdoInstance = xdo_new(nullptr);
    while (maxTries > 0) {
        if (xdo_search_windows(xdoInstance, &searchReq, &windowList, &windowsNumber)) {
            xdo_free(xdoInstance);
            qFatal("Error: Couldn't perform window search");
        }

        if (windowsNumber == 1) {
            xdo_free(xdoInstance);
            return WId(windowList[0]);
        } else if (windowsNumber > 1) {
            xdo_free(xdoInstance);
            qFatal("Error: Found %u window(s)", windowsNumber);
        }
        --maxTries;
        QThread::msleep(sleepTime);
    }
    xdo_free(xdoInstance);
    qFatal("Error: Found no window");
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
    QApplication::exit();
}
