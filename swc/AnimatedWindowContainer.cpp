#include "AnimatedWindowContainer.h"
#include "Settings.h"
#include <QThread>
#include <signal.h>

static xdo_search_t createSearchRequest();

AnimatedWindowContainer::AnimatedWindowContainer(
        std::unique_ptr<Settings> &&settings, WId windowId, QWidget *p) :
    AnimatedContainer(std::move(settings), p)
{
    xdoInstance = xdo_new(nullptr);
    embedWindow(windowId);
}

AnimatedWindowContainer::AnimatedWindowContainer(std::unique_ptr<Settings> &&settings, const QString &className, QWidget *p) :
    AnimatedContainer(std::move(settings), p)
{
    xdoInstance = xdo_new(nullptr);
    xdo_search_t searchReq = createSearchRequest();

    const char* windowPattern = className.toStdString().c_str();
    searchReq.winclassname = windowPattern;
    searchReq.searchmask = SEARCH_CLASSNAME;
    int maxTries = this->settings->getInt("lookup/max_tries");
    embedWindow(searchWindow(searchReq, maxTries));
}

AnimatedWindowContainer::AnimatedWindowContainer(std::unique_ptr<Settings> &&settings, std::unique_ptr<QProcess> &&process, QWidget *p) :
    AnimatedContainer(std::move(settings), p), executable(std::move(process))
{
    xdoInstance = xdo_new(nullptr);
    xdo_search_t searchReq = createSearchRequest();

    searchReq.pid = executable->processId();
    searchReq.searchmask = SEARCH_PID;
    int maxTries = this->settings->getInt("lookup/max_tries");
    embedWindow(searchWindow(searchReq, maxTries));
}

AnimatedWindowContainer::AnimatedWindowContainer(std::unique_ptr<Settings> &&settings, std::unique_ptr<QProcess> &&process, QString const& className, QWidget *p) :
    AnimatedContainer(std::move(settings), p), executable(std::move(process))
{
    xdoInstance = xdo_new(nullptr);
    xdo_search_t searchReq = createSearchRequest();

    const char* windowPattern = className.toStdString().c_str();
    searchReq.winclassname = windowPattern;
    searchReq.pid = executable->processId();
    searchReq.searchmask = SEARCH_PID | SEARCH_CLASSNAME;
    int maxTries = this->settings->getInt("lookup/max_tries");
    embedWindow(searchWindow(searchReq, maxTries));
}

AnimatedWindowContainer::~AnimatedWindowContainer()
{
    xdo_free(xdoInstance);
    if (executable && executable->state() == QProcess::Running) {
        executable->kill();
        executable->waitForFinished();
    }
}

void AnimatedWindowContainer::releaseWindow()
{
    existingWindow->setParent(nullptr);
    existingWindow->setFramePosition(this->pos());
    container->setParent(nullptr);

    existingWindow = nullptr;
}

void AnimatedWindowContainer::animate()
{
    // Resize the window to its original size
    // It is needed to avoid artifacts happening when the container is
    // created before the window is rendered
    existingWindow->resize(maximumGeometry.size());

    // Request change of state
    emit needAnimate();
}

void AnimatedWindowContainer::embedWindow(WId windowId)
{
    // Get the size
    QSize originalSize;
    const QString sizeType = settings->getString("container/size_type");
    if (sizeType == "absolute")
        originalSize = settings->getSize("container/size");
    else if (sizeType == "auto")
        originalSize = getWindowSize(windowId);
    else {
        qWarning("Error: size_type setting not recognized");
        return;
    }

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

    // Get the slide direction to define the anchor and the geometries
    const QString direction = settings->getString("animation/direction");
    if (direction == "down") {
        QPoint anchor = QPoint(originalSize.width() / 2, 0);
        minimumGeometry = QRect(pos.toPoint() - anchor,
                                QSize(originalSize.width(), 0));
        maximumGeometry = QRect(pos.toPoint() - anchor,
                                originalSize);
    } else if (direction == "up") {
        QPoint anchor = QPoint(originalSize.width() / 2, 0);
        minimumGeometry = QRect(pos.toPoint() - anchor,
                                QSize(originalSize.width(), 0));
        anchor.setY(originalSize.height());
        maximumGeometry = QRect(pos.toPoint() - anchor,
                                originalSize);
    } else if (direction == "right") {
        QPoint anchor = QPoint(0, originalSize.height() / 2);
        minimumGeometry = QRect(pos.toPoint() - anchor,
                                QSize(0, originalSize.height()));
        maximumGeometry = QRect(pos.toPoint() - anchor,
                                originalSize);
    } else if (direction == "left") {
        QPoint anchor = QPoint(0, originalSize.height() / 2);
        minimumGeometry = QRect(pos.toPoint() - anchor,
                                QSize(0, originalSize.height()));
        anchor.setX(originalSize.width());
        maximumGeometry = QRect(pos.toPoint() - anchor,
                                originalSize);
    } else {
        qWarning("Error: direction setting not recognized");
        return;
    }

    // Place the animated container
    // By default it window will be hidden
    // It will "slide" after a call to animate
    this->setGeometry(minimumGeometry);
    this->setMinimumSize(minimumGeometry.size());
    this->setMaximumSize(maximumGeometry.size());

    // Embed the window using the windowId
    existingWindow = QWindow::fromWinId(windowId);
    container = QWidget::createWindowContainer(existingWindow, this);
    container->resize(originalSize);

    // Initialize and start the state machine
    initSlideMachine();
    if (slideMachine)
        slideMachine->start();
}

void AnimatedWindowContainer::initSlideMachine()
{
    // Run parent "initSlideMachine"
    AnimatedContainer::initSlideMachine();

    // Maybe stop the process if the state becomes inactive
    // and resume it when it becomes active again
    if (settings->getBool("ownership/pause")) {
        connect(hiddenState, SIGNAL(entered()),
                this, SLOT(containerHidden()));
        connect(visibleState, SIGNAL(entered()),
                this, SLOT(containerShown()));
    }
}

xdo_search_t createSearchRequest()
{
    xdo_search_t searchReq;
    memset(&searchReq, 0, sizeof(xdo_search_t));
    searchReq.max_depth = -1;
    searchReq.require = xdo_search_t::SEARCH_ANY;
    searchReq.limit = 0;
    return searchReq;
}

WId AnimatedWindowContainer::searchWindow(const xdo_search_t &searchReq, int maxTries)
{
    Window *windowList = nullptr;
    unsigned int windowsNumber = 0;

    // Get the sleep time from settings
    const uint sleepTime = settings->getUInt("lookup/try_interval");
    if (sleepTime > 1000) {
        // TODO: raise exception instead of qFatal
        qFatal("Error: try_interval is bigger than 1000 in config file");
    }

    // Try maxTries times to find the window
    while (maxTries > 0) {
        if (xdo_search_windows(xdoInstance, &searchReq,
                               &windowList, &windowsNumber)) {
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

QSize AnimatedWindowContainer::getWindowSize(WId windowId) const
{
    unsigned int width, height;
    xdo_get_window_size(xdoInstance, windowId, &width, &height);
    return QSize(int(width), int(height));
}

void AnimatedWindowContainer::containerShown()
{
    if (executable)
        kill(int(executable->pid()), SIGCONT);
}

void AnimatedWindowContainer::containerHidden()
{
    if (executable)
        kill(int(executable->pid()), SIGSTOP);
}

void AnimatedWindowContainer::closeEvent(QCloseEvent *event)
{
    releaseWindow();
    event->accept();
    QApplication::exit();
}
