#ifndef SELFANIMATOR_H
#define SELFANIMATOR_H

#include <QtGui>
#include <QtWidgets>

extern "C" {
#include <xdo.h>
}

class AnimatedContainer: public QWidget
{
    Q_OBJECT
public:
    AnimatedContainer(WId windowId, QWidget *p = nullptr);
    AnimatedContainer(QString const& className, QWidget *p = nullptr);
    AnimatedContainer(int pid, QWidget *p = nullptr);
    AnimatedContainer(int pid, QString const& className, QWidget *p = nullptr);
    virtual ~AnimatedContainer() override;
    void releaseWindow();

public slots:
    void animate();

private:
    void embedWindow(WId windowId);
    xdo_search_t createSearchRequest();
    WId searchWindow(xdo_search_t const& searchReq);

    QPointer<QWindow> existingWindow;
    QPointer<QWidget> container;
    QSize originalSize;
    bool reverseSlide;

private slots:
    void slideInFinished();
    void pauseFinished();
    void slideOutFinished();
    void closeEvent(QCloseEvent *event) override;
};

#endif // SELFANIMATOR_H
