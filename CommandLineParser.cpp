#include "CommandLineParser.h"
#include "AnimatedContainer.h"
#include "cloptions.h"
#include <QtDBus>

CommandLineParser::CommandLineParser() :
    container(nullptr)
{
    parser.setApplicationDescription("Embed windows easily and animate them");
    parser.addPositionalArgument("swc-key",
                                 "The unique swc-key this container should use");
    parser.addHelpOption();

    parser.addOption(cloptions::className);
}

CommandLineParser::~CommandLineParser()
{
    if (container)
        delete container;
}

void CommandLineParser::process(const QCoreApplication &app)
{
    bool foundOption = false;
    parser.process(app);

    // Exit if no swc-key was given
    if (parser.positionalArguments().size() != 1) {
        qWarning("Error: no swc-key given.\n");
        parser.showHelp();
    }
    swcKey = "org.swc." + parser.positionalArguments().at(0);

    QDBusInterface iface(swcKey, "/", "", QDBusConnection::sessionBus());

    // Check the options for creating a container
    if (!foundOption && parser.isSet(cloptions::className)) {
        if (iface.isValid())
            qFatal("Trying to reuse an swc-key for a new container");
        container = new AnimatedContainer(parser.value(cloptions::className));
        foundOption = true;
    }

    // Exit if no option was found to create a container
    // AND there is no valid DBus to communicate with an existing one
    if (!foundOption && !iface.isValid()) {
        qWarning("Error: No existing container with this swc-key.\n");
        parser.showHelp();
    }

    // If we hold the container, prepare the DBus for receiving signals
    if (container) {
        // Initialize the DBus
        if (!QDBusConnection::sessionBus().registerService(swcKey))
            qFatal("Could not register service on DBus");
        QDBusConnection::sessionBus()
                .registerObject("/", container, QDBusConnection::ExportAllSlots);

    //    container->setWindowFlag(Qt::X11BypassWindowManagerHint);
        container->setWindowFlag(Qt::WindowStaysOnTopHint);
        container->show();
    }

    // Otherwise, we send a signal to the DBus
    else {
        auto reply = iface.call("animate");
    }
}

bool CommandLineParser::isOwningContainer() const
{
    return container != nullptr;
}
