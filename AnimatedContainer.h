#ifndef SELFANIMATOR_H
#define SELFANIMATOR_H

#include <QtGui>
#include <QtWidgets>

class AnimatedContainer: public QWidget
{
    Q_OBJECT
public:
    AnimatedContainer(WId windowId, QWidget *p = nullptr);
    AnimatedContainer(QString const& className, QWidget *p = nullptr);
    virtual ~AnimatedContainer() override;
    void releaseWindow();

private:
    void embedWindow(WId windowId);

    QPointer<QWindow> existingWindow;
    QPointer<QWidget> container;
    QSize originalSize;

private slots:
    void slideInFinished();
    void pauseFinished();
    void slideOutFinished();
    void closeEvent(QCloseEvent *event) override;
};

#endif // SELFANIMATOR_H
