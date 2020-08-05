#include "CommandLineParser.h"
#include "AnimatedContainer.h"
#include "exceptions.h"

#include <QApplication>
#include <QtDBus/QtDBus>

int main(int argc, char **argv)
{
    if (!QDBusConnection::sessionBus().isConnected()) {
        qFatal("Cannot connect to the D-Bus session bus.");
    }

    CommandLineParser parser;
    CommandLineParser::Result res;
    try {
        res = parser.process(argc, argv);
    } catch (const exceptions::XdoError &ex) {
        qWarning(ex.what());
        return 1;
    }

    if (res.container) {
        assert(res.app.get());
        return res.app->exec();
    }
    return 0;
}
