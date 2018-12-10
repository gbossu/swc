#ifndef SELFANIMATOR_H
#define SELFANIMATOR_H

#include <QtGui>
#include <QtWidgets>
#include <iostream>

class AnimatedContainer: public QWidget
{
    Q_OBJECT
public:
    AnimatedContainer(WId windowId, QWidget *p = nullptr);
    AnimatedContainer(QString const& className, QWidget *p = nullptr);

private:
    void embedWindow(WId windowId);

private slots:
    void slideInFinished();
    void pauseFinished();
    void slideOutFinished();
};

#endif // SELFANIMATOR_H
