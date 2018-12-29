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
/// @}
}
#endif // COMMANDLINEOPTIONS_H
