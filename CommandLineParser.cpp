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
    parser.addOption(cloptions::pid);
    parser.addOption(cloptions::binary);
}

CommandLineParser::~CommandLineParser()
{
    if (container)
        delete container;
    if (executable.state() == QProcess::Running) {
        executable.kill();
        executable.waitForFinished();
    }
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

    // Otherwise, try to "connect" to an existing swc instance
    swcKey = "org.swc." + parser.positionalArguments().at(0);
    QDBusInterface iface(swcKey, "/", "", QDBusConnection::sessionBus());

    // Check the options for creating a container
    if (parser.isSet(cloptions::binary)) {
        if (iface.isValid()) {
            qWarning("Error: Trying to reuse an swc-key for a new container.");
            return;
        }

        // Extract the program and its arguments
        QStringList args = parser.value(cloptions::binary).split(" ");
        if (args.size() == 0) {
            qWarning("Error: No binary file was given.");
            return;
        }
        executable.setProgram(args.front());
        args.pop_front();
        executable.setArguments(args);
        executable.start();
        executable.waitForStarted();

        // Then create the container using the PID (and maybe the classname
        // as well).
        container = parser.isSet(cloptions::className) ?
                    new AnimatedContainer(&settings,
                                          int(executable.processId()),
                                          parser.value(cloptions::className)) :
                    new AnimatedContainer(&settings,
                                          int(executable.processId()));
        foundOption = true;
    } else if (parser.isSet(cloptions::pid)) {
        if (iface.isValid()) {
            qWarning("Error: Trying to reuse an swc-key for a new container.");
            return;
        }
        container = new AnimatedContainer(&settings,
                                          parser.value(cloptions::pid).toInt());
        foundOption = true;
    } else if (parser.isSet(cloptions::className)) {
        if (iface.isValid()) {
            qWarning("Error: Trying to reuse an swc-key for a new container.");
            return;
        }
        container = new AnimatedContainer(&settings,
                                          parser.value(cloptions::className));
        foundOption = true;
    }

    // Exit if no option was found to create a container
    // AND there is no valid DBus to communicate with an existing one
    if (!foundOption && !iface.isValid()) {
        qWarning("Error: No existing container with this swc-key.");
        return;
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
    return container != nullptr && container->hasWindow();
}
