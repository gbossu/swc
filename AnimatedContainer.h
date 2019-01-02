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
    xdo_search_t createSearchRequest();
    WId searchWindow(xdo_search_t const& searchReq, int maxTries);

    QPointer<QWindow> existingWindow;
    QPointer<QWidget> container;
    Settings *settings;
    QSize originalSize;
    bool reverseSlide;

private slots:
    void slideInFinished();
    void pauseFinished();
    void slideOutFinished();
    void closeEvent(QCloseEvent *event) override;
};

#endif // SELFANIMATOR_H
