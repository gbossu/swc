#include "CommandLineParser.h"
#include "AnimatedContainer.h"
#include "Settings.h"
#include "cloptions.h"
#include <QtDBus>
#include <iostream>

CommandLineParser::CommandLineParser() :
    container(nullptr), settings(nullptr)
{
    parser.addPositionalArgument("swc-key",
                                 "The unique swc-key this container should use");

    // Help options
    parser.addOption(cloptions::help);
    parser.addOption(cloptions::version);

    // Input options
    parser.addOption(cloptions::className);
    parser.addOption(cloptions::pid);
    parser.addOption(cloptions::binary);

    // Additional options
    parser.addOption(cloptions::size);
    parser.addOption(cloptions::position);
    parser.addOption(cloptions::direction);
    parser.addOption(cloptions::pause);
}

CommandLineParser::~CommandLineParser()
{
    if (container)
        delete container;
    if (settings)
        delete settings;
    if (executable.state() == QProcess::Running) {
        executable.kill();
        executable.waitForFinished();
    }
}

void CommandLineParser::process(const QCoreApplication &app)
{
    parser.process(app);

    /********
     * Process help options
     ********/

    if (parser.isSet(cloptions::help)) {
        showHelp();
        return;
    } else if (parser.isSet(cloptions::version)) {
        showVersion();
        return;
    }


    /********
     * Process positional arguments
     ********/

    // Exit if no swc-key was given
    if (parser.positionalArguments().size() != 1) {
        qWarning("Error: no swc-key given.\n");
        showHelp();
        return;
    }

    // Otherwise, try to "connect" to an existing swc instance
    swcKey = parser.positionalArguments().at(0);
    QString dbusKey = "org.swc." + swcKey;
    QDBusInterface iface(dbusKey, "/", "", QDBusConnection::sessionBus());

    // If only an swc-key was given, and if an swc instance exists with
    // this name, then we send an "animate" request
    if (parser.optionNames().empty()) {
        if (iface.isValid())
            iface.call("animate");
        else
            qWarning("Error: No existing container with this swc-key.");
        return;
    }


    /********
     * Process cutomization options
     ********/

    // First create the Settings object
    settings = new Settings(swcKey);

    if (parser.isSet(cloptions::size)) {
        auto values = parser.value(cloptions::size).split(",");
        if (values.size() == 2) {
            settings->setValue("container/size_type", "absolute");
            settings->setValue("container/size", QSize(values.at(0).toInt(),
                                                      values.at(1).toInt()));
        } else if (values.size() == 1 && values.at(0) == "auto") {
            settings->setValue("container/size_type", "auto");
        } else {
            qWarning("Error: invalid format for size, please use "
                     "either 'width,height' or 'auto'");
            return;
        }
    }

    if (parser.isSet(cloptions::position)) {
        auto values = parser.value(cloptions::position).split(",");
        if (values.size() != 2) {
            qWarning("Error: invalid format for position, please use 'x,y'");
            return;
        }
        settings->setValue("container/position", QSize(values.at(0).toInt(),
                                                      values.at(1).toInt()));
        settings->setValue("container/position_type", "absolute");
    }

    if (parser.isSet(cloptions::direction)) {
        auto value = parser.value(cloptions::direction);
        if (value == "d")
            settings->setValue("animation/direction", "down");
        else if (value == "u")
            settings->setValue("animation/direction", "up");
        else if (value == "r")
            settings->setValue("animation/direction", "right");
        else if (value == "l")
            settings->setValue("animation/direction", "left");
        else {
            qWarning("Error: unknown slide direction");
            return;
        }
    }

    if (parser.isSet(cloptions::pause))
        settings->setValue("ownership/pause", true);


    /********
     * Process input options
     ********/

    // Check all the supported input options for creating a container
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

        // Then create the container using the PID
        // (and maybe the classname as well).
        container = parser.isSet(cloptions::className) ?
                    new AnimatedContainer(settings,
                                          int(executable.processId()),
                                          parser.value(cloptions::className)) :
                    new AnimatedContainer(settings,
                                          int(executable.processId()));
        container->setExecutalbe(&executable);
    } else if (parser.isSet(cloptions::pid)) {
        if (iface.isValid()) {
            qWarning("Error: Trying to reuse an swc-key for a new container.");
            return;
        }
        container = new AnimatedContainer(settings,
                                          parser.value(cloptions::pid).toInt());
    } else if (parser.isSet(cloptions::className)) {
        if (iface.isValid()) {
            qWarning("Error: Trying to reuse an swc-key for a new container.");
            return;
        }
        container = new AnimatedContainer(settings,
                                          parser.value(cloptions::className));
    }  else {
        // Exit if no option was found to create a container
        if (iface.isValid())
            qWarning("Error: Options were provided for toggling a container.");
        else
            qWarning("Error: No input option was provided to create a container.");
        return;
    }


    /********
     * Register a service on the DBus
     ********/

    // If we created a valid container, prepare the DBus for receiving signals
    if (container->hasWindow()) {

        // Initialize the DBus
        if (!QDBusConnection::sessionBus().registerService(dbusKey))
            qFatal("Could not register service on DBus");
        QDBusConnection::sessionBus()
                .registerObject("/", container, QDBusConnection::ExportAllSlots);

        // Finally display the container
        container->show();
    }

    // Otherwise, something failed when creating the container
    else {
        // Do nothing for now
    }
}

bool CommandLineParser::isOwningContainer() const
{
    return container != nullptr && container->hasWindow();
}

void CommandLineParser::showHelp() const
{
    // It is easier not to use QStrings and the "printable" stuff
    std::cout << cloptions::helpText;
}

void CommandLineParser::showVersion() const
{
    std::cout << "0.1" << std::endl;
}
