#ifndef SELFANIMATOR_H
#define SELFANIMATOR_H

#include <QtGui>
#include <QtWidgets>
#include <iostream>

class AnimatedContainer: public QWidget
{
    Q_OBJECT
public:
    AnimatedContainer(QWidget *p = nullptr);

private slots:
    void slideInFinished();
    void pauseFinished();
    void slideOutFinished();
};

#endif // SELFANIMATOR_H
