#include "CommandLineParser.h"
#include "AnimatedContainer.h"
#include <QApplication>
#include <QtDBus/QtDBus>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    QApplication::setApplicationName("SWC");

    if (!QDBusConnection::sessionBus().isConnected()) {
        qFatal("Cannot connect to the D-Bus session bus.");
    }

    CommandLineParser parser;
    if (std::unique_ptr<AnimatedContainer> container = parser.process(app)) {
        return app.exec();
    }
    return 0;
}
