#ifndef COMMANDLINEOPTIONS_H
#define COMMANDLINEOPTIONS_H

#include <QCommandLineOption>

namespace cloptions {

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
/// @}
}
#endif // COMMANDLINEOPTIONS_H
