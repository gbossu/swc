#include "AnimatedWindowContainer.h"
#include "Settings.h"
#include <QThread>
#include <signal.h>

static xdo_search_t createSearchRequest();

AnimatedWindowContainer::AnimatedWindowContainer(
        std::unique_ptr<Settings> &&settings, WId windowId) :
    AnimatedContainer(std::move(settings))
{
    xdoInstance = xdo_new(nullptr);
    setupWindowContainer(windowId);
}

AnimatedWindowContainer::AnimatedWindowContainer(
        std::unique_ptr<Settings> &&settings, const QString &className) :
    AnimatedContainer(std::move(settings))
{
    xdoInstance = xdo_new(nullptr);
    xdo_search_t searchReq = createSearchRequest();

    const char* windowPattern = className.toStdString().c_str();
    searchReq.winclassname = windowPattern;
    searchReq.searchmask = SEARCH_CLASSNAME;
    int maxTries = this->settings->getInt("lookup/max_tries");
    setupWindowContainer(searchWindow(searchReq, maxTries));
}

AnimatedWindowContainer::AnimatedWindowContainer(
        std::unique_ptr<Settings> &&settings,
        std::unique_ptr<QProcess> &&process) :
    AnimatedContainer(std::move(settings)), executable(std::move(process))
{
    xdoInstance = xdo_new(nullptr);
    xdo_search_t searchReq = createSearchRequest();

    searchReq.pid = executable->processId();
    searchReq.searchmask = SEARCH_PID;
    int maxTries = this->settings->getInt("lookup/max_tries");
    setupWindowContainer(searchWindow(searchReq, maxTries));
}

AnimatedWindowContainer::AnimatedWindowContainer(
        std::unique_ptr<Settings> &&settings,
        std::unique_ptr<QProcess> &&process, QString const& className) :
    AnimatedContainer(std::move(settings)), executable(std::move(process))
{
    xdoInstance = xdo_new(nullptr);
    xdo_search_t searchReq = createSearchRequest();

    const char* windowPattern = className.toStdString().c_str();
    searchReq.winclassname = windowPattern;
    searchReq.pid = executable->processId();
    searchReq.searchmask = SEARCH_PID | SEARCH_CLASSNAME;
    int maxTries = this->settings->getInt("lookup/max_tries");
    setupWindowContainer(searchWindow(searchReq, maxTries));
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

    AnimatedContainer::animate();
}

void AnimatedWindowContainer::setupWindowContainer(WId windowId)
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

    // Find and create a container for the window
    existingWindow = QWindow::fromWinId(windowId);
    container = QWidget::createWindowContainer(existingWindow, this);
    container->resize(originalSize);

    setupContainerWidget(originalSize);

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
