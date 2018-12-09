#include "AnimatedContainer.h"
#include <QApplication>
#include <QtGui>
#include <QtWidgets>
#include <iostream>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

//    if (app.arguments().count() != 2) {
//        qFatal("Error: expected executable path as argument");
//        return 1;
//    }

    AnimatedContainer w;
//    w.setWindowFlag(Qt::X11BypassWindowManagerHint);
    w.setWindowFlag(Qt::WindowStaysOnTopHint);
//    w.setWindowFlag(Qt::Desktop);
//    w.setWindowFlag(Qt::BypassWindowManagerHint);
    w.show();
    return app.exec();
}
