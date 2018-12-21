#include "AnimatedContainer.h"
#include <QApplication>
#include <QtGui>
#include <QtWidgets>

int main(int argc, char **argv)
{
    QPointer<AnimatedContainer> container = nullptr;
    bool foundOption = false;

    QApplication app(argc, argv);
    QApplication::setApplicationName("SWC");

    QCommandLineParser parser;
    parser.setApplicationDescription("Embed windows easily and animate them");
    parser.addHelpOption();

    QCommandLineOption classNameOption(QStringList() << "c" << "class-name",
                                       "Class name of the window to embed",
                                       "className");
    parser.addOption(classNameOption);

    parser.process(app);
    if (!foundOption && parser.isSet(classNameOption)) {
        container = new AnimatedContainer(parser.value(classNameOption));
        foundOption = true;
    }

    // Exit if no option was found
    if (!foundOption) {
        parser.showHelp();
    }

//    container->setWindowFlag(Qt::X11BypassWindowManagerHint);
    container->setWindowFlag(Qt::WindowStaysOnTopHint);
//    container->setWindowFlag(Qt::Desktop);
//    container->setWindowFlag(Qt::BypassWindowManagerHint);
    container->show();

    return app.exec();
}
