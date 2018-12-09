#include "AnimatedContainer.h"

AnimatedContainer::AnimatedContainer(QWidget *p) : QWidget(p)
{
    auto size = QSize(360, 400);
    auto width = QApplication::desktop()->width();

    auto existingWindow = QWindow::fromWinId(37748738);
    QWidget *container = QWidget::createWindowContainer(existingWindow, this);
//    container->setFocusPolicy(Qt::TabFocus);
    container->resize(size);
    container->show();

    auto windowFlags = Qt::WindowFlags(Qt::BypassWindowManagerHint);
//    container->setWindowFlags(windowFlags);

    // Start a slide from the top of the screen
    QPropertyAnimation *slideIn = new QPropertyAnimation(this, "pos");
    slideIn->setEasingCurve(QEasingCurve(QEasingCurve::OutQuad));
    slideIn->setDuration(400);
    slideIn->setStartValue(QPoint(width / 2, -200));
    slideIn->setEndValue(QPoint(width / 2, 0));
    slideIn->start();
    connect(slideIn, SIGNAL(finished()), SLOT(slideInFinished()));
    connect(slideIn, SIGNAL(finished()), slideIn, SLOT(deleteLater()));
}

void AnimatedContainer::slideInFinished()
{
    // Wait some time
    QTimer::singleShot(3000, this, SLOT(pauseFinished()));
}

void AnimatedContainer::pauseFinished()
{
    //        connect(slideOut, SIGNAL(finished()), SLOT(slideOutFinished()));
    //        connect(slideOut, SIGNAL(finished()), slideOut, SLOT(deleteLater()));
}

void AnimatedContainer::slideOutFinished()
{
    // Do nothing yet
}
