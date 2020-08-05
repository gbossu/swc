#pragma once

#include <memory>
#include <QCommandLineParser>

class QApplication;

class AnimatedContainer;

class CommandLineParser
{
public:

    /// The return type of @ref process.
    /// It will have default values if the command line options were invalid or
    /// did not contain an input option.
    struct Result {
        std::unique_ptr<AnimatedContainer> container = nullptr;
        std::unique_ptr<QApplication> app = nullptr;
    };

    /**
     * Creates a helper object to parse command-line arguments.
     *
     * It uses the options defined in @ref cloptions
     */
    CommandLineParser();

    /**
     * Processes the app arguments and acts accordingly.
     *
     * @param argc The number of command-line arguments
     * @param argc The list of command-line arguments
     * @return An AnimatedContainer and QApplication if the program args had
     *         valid input options.
     */
    Result process(int &argc, char **argv);

private:
    void showHelp(bool full = false) const;
    void showVersion() const;

    QCommandLineParser parser;
};
