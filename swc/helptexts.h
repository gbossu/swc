#ifndef HELPTEXTS_H
#define HELPTEXTS_H

#include <string>

namespace helptexts {

    const std::string usage =
        "Usage: swc <swc-key>\n"
        " or:   swc <input-option> [customization-options] <swc-key>\n"
        " or:   swc {-h [full] | -v}\n";

    const std::string description =
        "swc embeds windows easily and animates them.\n"
        "First you should create an swc container to embed your window, see the"
        " examples below. You have to give it a unique swc-key (a string). This"
        " key will be used by other instances of swc when you want to animate "
        "an existing swc container.\n";

    const std::string examples =
        "Examples:\n"
        "  toggle existing container  swc <swc-key>\n"
        "  create new container       swc <input-option> [custom-options] <swc-key>\n";

    const std::string options =
        "Options:\n"
        "Input options:\n"
        "  -c, --class-name <name>  Class name of the window to embed.\n"
        "  -p, --pid <PID>          Id of the process which holds the window to embed.\n"
        "  -b, --binary <command>   Binary file that should be embedded.\n"
        "                           <command> uses the format \"<file> [args]\".\n"
        "  -m, --module <file>      JSON describing the modules to create.\n"
        "Customization options:\n"
        "  -S, --size <size>        Size of the container. <size> can be w,h or auto.\n"
        "  -P, --position <pos>     Position of the container. <pos> is x,y.\n"
        "  -D, --direction d|u|r|l  Direction of the animation.\n"
        "  -I, --pause              Pause the process when the container is hidden.\n";
}
#endif // HELPTEXTS_H
