#ifndef COMMANDLINEPARSER_H
#define COMMANDLINEPARSER_H

#include <QCommandLineParser>
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
     * Properly detructs the parser by detaching shared memory
     */
    ~CommandLineParser();

    /**
     * Processes the app arguments and acts accordingly.
     *
     * @param app The Qt application
     */
    void process(QCoreApplication const& app);

    bool isOwningContainer() const;

private:
    AnimatedContainer *container;
    bool owningContainer;
    QString swcKey;
    QCommandLineParser parser;
};

#endif // COMMANDLINEPARSER_H
