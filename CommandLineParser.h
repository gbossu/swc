#ifndef COMMANDLINEPARSER_H
#define COMMANDLINEPARSER_H

#include <QCommandLineParser>
#include <QPointer>
#include <QCoreApplication>

class AnimatedContainer;

class CommandLineParser
{
public:

    /**
     * Creates a helper object to parse command-line arguments.
     *
     * It uses the options defined in @ref cloptions
     */
    CommandLineParser();

    /**
     * Processes the app arguments and acts accordingly.
     *
     * @param app The Qt application
     */
    void process(QCoreApplication const& app);

private:
    QPointer<AnimatedContainer> container;
    QCommandLineParser parser;
};

#endif // COMMANDLINEPARSER_H
