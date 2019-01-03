#include "AnimatedContainer.h"
#include "Settings.h"
#include <QThread>

AnimatedContainer::AnimatedContainer(Settings *settings, WId windowId, QWidget *p) :
    QWidget(p), settings(settings)
{
    xdoInstance = xdo_new(nullptr);
    embedWindow(windowId);
}

AnimatedContainer::AnimatedContainer(Settings *settings, const QString &className, QWidget *p) :
    QWidget(p), settings(settings)
{
    xdoInstance = xdo_new(nullptr);
    xdo_search_t searchReq = createSearchRequest();

    const char* windowPattern = className.toStdString().c_str();
    searchReq.winclassname = windowPattern;
    searchReq.searchmask = SEARCH_CLASSNAME;
    embedWindow(searchWindow(searchReq, settings->getInt("lookup/max_tries")));
}

AnimatedContainer::AnimatedContainer(Settings *settings, int pid, QWidget *p) :
    QWidget(p), settings(settings)
{
    xdoInstance = xdo_new(nullptr);
    xdo_search_t searchReq = createSearchRequest();

    searchReq.pid = pid;
    searchReq.searchmask = SEARCH_PID;
    embedWindow(searchWindow(searchReq, settings->getInt("lookup/max_tries")));
}

AnimatedContainer::AnimatedContainer(Settings *settings, int pid, QString const& className, QWidget *p) :
    QWidget(p), settings(settings)
{
    xdoInstance = xdo_new(nullptr);
    xdo_search_t searchReq = createSearchRequest();

    const char* windowPattern = className.toStdString().c_str();
    searchReq.winclassname = windowPattern;
    searchReq.pid = pid;
    searchReq.searchmask = SEARCH_PID | SEARCH_CLASSNAME;
    embedWindow(searchWindow(searchReq, settings->getInt("lookup/max_tries")));
}

AnimatedContainer::~AnimatedContainer()
{
    xdo_free(xdoInstance);
}

void AnimatedContainer::releaseWindow()
{
    existingWindow->setParent(nullptr);
    existingWindow->setFramePosition(this->pos());
    container->setParent(nullptr);

    existingWindow = nullptr;
}

bool AnimatedContainer::hasWindow() const
{
    return container != nullptr;
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
    // Get the size
    const QString sizeType = settings->getString("container/size_type");
    if (sizeType == "absolute")
        originalSize = settings->getSize("container/size");
    else if (sizeType == "auto")
        originalSize = getWindowSize(windowId);
    else {
        qWarning("Error: size_type setting not recognized");
        return;
    }
    auto size = QSize(originalSize.width(), 0);

    // Get the position
    QPointF pos;
    const QString positionType = settings->getString("container/position_type");
    if (positionType == "absolute")
        pos = settings->getPoint("container/position");
    else if (positionType == "percent") {
        auto desktopSize = QApplication::desktop()->size();
        auto percents = settings->getPoint("container/position");
        pos.setX(desktopSize.width() * (percents.x() / 100.));
        pos.setY(desktopSize.height() * (percents.y() / 100.));
    } else {
        qWarning("Error: position_type setting not recognized");
        return;
    }

    // Place the animated container
    this->setMinimumSize(size);
    this->setMaximumSize(originalSize);
    this->move(pos.toPoint());

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
    if (tryInterval < 0) {
        // TODO: raise exception instead of qFatal
        qFatal("Error: negative try_interval time in config file");
    }
    const unsigned int sleepTime = static_cast<unsigned int>(tryInterval);

    // Try maxTries times to find the window
    while (maxTries > 0) {
        if (xdo_search_windows(xdoInstance, &searchReq, &windowList, &windowsNumber)) {
            // TODO: raise exception instead of qFatal
            qFatal("Error: Couldn't perform window search");
        }

        if (windowsNumber == 1)
            return WId(windowList[0]);
        else if (windowsNumber > 1) {
            // TODO: raise exception instead of qFatal
            qFatal("Error: Found %u window(s)", windowsNumber);
        }
        --maxTries;
        QThread::msleep(sleepTime);
    }

    // If we end up here, it means we found no window after maxTries tries
    // TODO: raise exception instead of qFatal
    qFatal("Error: Found no window");
}

QSize AnimatedContainer::getWindowSize(WId windowId) const
{
    unsigned int width, height;
    xdo_get_window_size(xdoInstance, windowId, &width, &height);
    return QSize(int(width), int(height));
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
