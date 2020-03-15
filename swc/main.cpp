#include "CommandLineParser.h"
#include "AnimatedContainer.h"
#include "exceptions.h"

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
    std::unique_ptr<AnimatedContainer> container;
    try {
        container = parser.process(app);
    } catch (const exceptions::XdoError &ex) {
        qWarning(ex.what());
        return 1;
    }

    if (container) {
        return app.exec();
    }
    return 0;
}
