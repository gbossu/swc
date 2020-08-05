#include "CommandLineParser.h"
#include "AnimatedWindowContainer.h"
#include "AnimatedModuleContainer.h"
#include "Settings.h"
#include "cloptions.h"
#include "helptexts.h"

#include <QtDBus>
#include <iostream>

CommandLineParser::CommandLineParser()
{
    // Help options
    parser.addOption(cloptions::help);
    parser.addOption(cloptions::version);

    // Input options
    parser.addOption(cloptions::className);
    parser.addOption(cloptions::pid);
    parser.addOption(cloptions::binary);
    parser.addOption(cloptions::modules);

    // Additional options
    parser.addOption(cloptions::size);
    parser.addOption(cloptions::position);
    parser.addOption(cloptions::direction);
    parser.addOption(cloptions::pause);
}

static QString getDBusKey(const QString &swcKey)
{
    return "org.swc." + swcKey;
}

static QDBusInterface getDBusInterface(const QString &swcKey){
    QString dbusKey = getDBusKey(swcKey);
    return QDBusInterface(dbusKey, "/", "", QDBusConnection::sessionBus());
}

CommandLineParser::Result
CommandLineParser::process(int &argc, char **argv)
{
    // First, try to parse the args without creating a QApplication.
    // This turns out to be quite important in the following scenario:
    //  - an swc container (with key "my-key") is the active window
    //  - there is a keyboard shortcut to run "swc my-key"
    //  - the hide_on_focus_loss option is enabled
    // Then if a QApplication is created when the user presses the shortcut,
    // it might first render the container window inactive, and it will
    // start sliding back because it lost focus. But then the signal to
    // toggle the container is sent, and it will start sliding in again.
    // In the end, nothing happened. One (temporary?) solution is to avoid
    // creating a QApplication when we know a GUI is not needed.
    QStringList args;
    for (int idx = 0; idx < argc; ++idx) {
        args.append(argv[idx]);
    }
    if (parser.parse(args) && parser.positionalArguments().size() == 1 &&
        parser.optionNames().size() == 0) {
        // If only an swc-key was passed, try to toggle its corresponding
        // container.
        QString swcKey = parser.positionalArguments().at(0);
        QDBusInterface iface = getDBusInterface(swcKey);
        if (iface.isValid())
            iface.call("animate");
        else
            qWarning("Error: No existing container with this swc-key.");
        return {};
    }

    // For other cases, create a QApplication
    auto app = std::make_unique<QApplication>(argc, argv);
    app->setApplicationName("swc");
    parser.process(*app);


    /********
     * Process help options
     ********/

    if (parser.isSet(cloptions::help)) {
        showHelp(parser.positionalArguments().size() == 1 &&
                 parser.positionalArguments().at(0) == "full");
        return {};
    } else if (parser.isSet(cloptions::version)) {
        showVersion();
        return {};
    }


    /********
     * Process positional arguments
     ********/

    // Exit if no swc-key was given
    if (parser.positionalArguments().size() != 1) {
        qWarning("Error: no swc-key given.\n");
        showHelp();
        return {};
    }

    // If only an swc-key was given and we get to this point, it means that
    // parser.parse() failed.
    if (parser.optionNames().empty()) {
        qWarning("Error: Qt-specific options were passed to toggle a "
                 "swc container.");
        return {};
    }


    /********
     * Process cutomization options
     ********/

    // First create the Settings object
    QString swcKey = parser.positionalArguments().at(0);
    auto settings = std::make_unique<Settings>(swcKey);

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
            return {};
        }
    }

    if (parser.isSet(cloptions::position)) {
        auto values = parser.value(cloptions::position).split(",");
        if (values.size() != 2) {
            qWarning("Error: invalid format for position, please use 'x,y'");
            return {};
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
            return {};
        }
    }

    if (parser.isSet(cloptions::pause))
        settings->setValue("ownership/pause", true);


    /********
     * Process input options
     ********/
    std::unique_ptr<AnimatedContainer> container;

    if (parser.isSet(cloptions::binary)) {
        // Extract the program and its arguments
        QStringList args = parser.value(cloptions::binary).split(" ");
        if (args.size() == 0) {
            qWarning("Error: No binary file was given.");
            return {};
        }

        auto executable = std::make_unique<QProcess>();
        executable->setProgram(args.front());
        args.pop_front();
        executable->setArguments(args);
        executable->start();
        executable->waitForStarted();

        // Then create the container using the PID
        // (and maybe the classname as well).
        container = parser.isSet(cloptions::className) ?
            std::make_unique<AnimatedWindowContainer>(
                std::move(settings), std::move(executable),
                parser.value(cloptions::className)) :
            std::make_unique<AnimatedWindowContainer>(
                std::move(settings), std::move(executable));
    } else if (parser.isSet(cloptions::pid)) {
        container = std::make_unique<AnimatedWindowContainer>(
            std::move(settings), parser.value(cloptions::pid).toInt());
    } else if (parser.isSet(cloptions::className)) {
        container = std::make_unique<AnimatedWindowContainer>(
            std::move(settings), parser.value(cloptions::className));
    } else if (parser.isSet(cloptions::modules)) {
        std::string moduleInfosPath =
            parser.value(cloptions::modules).toStdString();
        container = std::make_unique<AnimatedModuleContainer>(
            std::move(settings), moduleInfosPath);
    } else {
        // Exit if no option was found to create a container
        if (getDBusInterface(swcKey).isValid())
            qWarning("Error: Options were provided for toggling a container.");
        else
            qWarning("Error: No input option was provided to create a container.");
        return {};
    }


    /********
     * Register a service on the DBus
     ********/

    if (getDBusInterface(swcKey).isValid()) {
        qWarning("Error: Trying to reuse an swc-key for a new container.");
        return {};
    }

    // If we created a valid container, prepare the DBus for receiving signals
    if (!QDBusConnection::sessionBus().registerService(getDBusKey(swcKey)))
        qFatal("Could not register service on DBus");
    QDBusConnection::sessionBus()
            .registerObject("/", container.get(), QDBusConnection::ExportAllSlots);

    // Finally display the container
    container->show();
    return {std::move(container), std::move(app)};
}

void CommandLineParser::showHelp(bool full) const
{
    // It is easier not to use QStrings and the "printable" stuff

    if (full) {
        std::cout << helptexts::description << std::endl
                  << helptexts::examples << std::endl;
    }
    std::cout << helptexts::usage << std::endl
              << helptexts::options;
}

void CommandLineParser::showVersion() const
{
    std::cout << "0.1" << std::endl;
}
