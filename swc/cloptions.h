#ifndef COMMANDLINEOPTIONS_H
#define COMMANDLINEOPTIONS_H

#include <QCommandLineOption>

// WARNING: Do not forget to update the help messages in helptexts.h

namespace cloptions {

/// @name Help options
/// @{

    /** Show the help */
    const QCommandLineOption help(
            QStringList() << "h" << "help",
            "Display this help");

    /** Show swc's version */
    const QCommandLineOption version(
            QStringList() << "v" << "version",
            "Display the version");
/// @}

/// @name Options for creating the container
/// @{

    /** Create using the class-name of a window */
    const QCommandLineOption className(
            QStringList() << "c" << "class-name",
            "Class name of the window to embed",
            "className");

    /** Create using the pid of a process */
    const QCommandLineOption pid(
            QStringList() << "p" << "pid",
            "Id of the process which holds the window to embed",
            "pid");

    /** Create using a binary file */
    const QCommandLineOption binary(
            QStringList() << "b" << "binary",
            "Binary file (and its parameters) that should be embedded",
            "binary");

    /** Create using a module */
    const QCommandLineOption modules(
            QStringList() << "m" << "modules",
            "JSON describing the modules to create.",
            "modules");
/// @}

/// @name Additional options for customizing container
/// @{

    /** Specify the size of the container */
    const QCommandLineOption size(
            QStringList() << "S" << "size",
            "Size of the container ('w,h' or 'auto')",
            "size");

    /** Specify the position of the container */
    const QCommandLineOption position(
            QStringList() << "P" << "position",
            "Position of the container ('x,y')",
            "position");

    /** Specify the direction of the slide animation */
    const QCommandLineOption direction(
            QStringList() << "D" << "direction",
            "direction of the slide animation (d|u|r|l)",
            "direction");

    /** Show swc's version */
    const QCommandLineOption pause(
            QStringList() << "I" << "pause",
            "Pause the process when the container is hidden");
/// @}
}
#endif // COMMANDLINEOPTIONS_H
