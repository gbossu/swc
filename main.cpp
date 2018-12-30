#include "CommandLineParser.h"
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
    parser.process(app);

    if (parser.isOwningContainer())
        return app.exec();
    else
        return 0;
}
