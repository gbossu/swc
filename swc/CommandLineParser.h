#ifndef COMMANDLINEPARSER_H
#define COMMANDLINEPARSER_H

#include <QCommandLineParser>
#include <QCoreApplication>
#include <QProcess>
#include "Settings.h"

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
     * Properly detructs the parser by detaching shared memory
     */
    ~CommandLineParser();

    /**
     * Processes the app arguments and acts accordingly.
     *
     * @param app The Qt application
     */
    void process(QCoreApplication const& app);

    /**
     * @return true if this swc instance is owning a valid container
     */
    bool isOwningContainer() const;

private:
    void showHelp() const;
    void showVersion() const;

    AnimatedContainer *container;
    Settings settings;
    QString swcKey;
    QCommandLineParser parser;
    QProcess executable;
};

#endif // COMMANDLINEPARSER_H
