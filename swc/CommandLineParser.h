#pragma once

#include <memory>
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QProcess>

class AnimatedContainer;
class Settings;

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
     * @return An AnimatedContainer if the program args had valid input options
     */
    std::unique_ptr<AnimatedContainer> process(QCoreApplication const& app);

private:
    void showHelp(bool full = false) const;
    void showVersion() const;

    QString swcKey;
    QCommandLineParser parser;
};
