#ifndef COMMANDLINEOPTIONS_H
#define COMMANDLINEOPTIONS_H

#include <QCommandLineOption>

namespace cloptions {

/// @name Help options
/// @{

    /** Show the help */
    const QCommandLineOption help(
            QStringList() << "h" << "help",
            "Display this help");

    /** Create using the pid of a process */
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
/// @}

    const std::string helpText =
        "Usage: swc <swc-key>\n"
        " or:   swc <input-option> [customization-options] <swc-key>\n"
        " or:   swc {-h|-v}\n"
        "\n"
        "swc: Embed windows easily and animate them\n"
        "First you should create an swc container to embed your window, see the"
        " examples below. You have to give it a unique swc-key (a string). This"
        " key will be used by other instances of swc when you want to animate "
        "an existing swc container.\n"
        "\n"
        "Examples:\n"
        "  toggle existing container  swc <swc-key>\n"
        "  create new container       swc <input-option> [custom-options] <swc-key>\n"
        "\n"
        "Options:\n"
        "Input options:\n"
        "  -c, --class-name <name>  Class name of the window to embed\n"
        "  -p, --pid <PID>          Id of the process which holds the window to embed\n"
        "  -b, --binary <command>   Binary file that should be embedded\n"
        "                           <command> uses the format \"<file> [args]\"\n"
        "Customization options:\n"
        "  -S, --size <size>        Size of the container. <size> can be w,h or auto\n"
        "  -P, --position <pos>     Position of the container. <pos> is x,y\n"
        "  -D, --direction d|u|r|l  Direction of the animation\n";
}
#endif // COMMANDLINEOPTIONS_H
