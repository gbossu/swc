#ifndef HELPTEXTS_H
#define HELPTEXTS_H

#include <string>

namespace helptexts {

    const std::string usage =
        "Usage: swc <swc-key>\n"
        " or:   swc <input-option> [customization-options] <swc-key>\n"
        " or:   swc {-h [full] | -v}\n";

    const std::string description =
        "swc embeds windows or modules/widgets in a container easily and animates it.\n"
        "The first step is to create the swc-container. The latter can either embed a whole window (e.g. your favorite terminal emulator, to create a drop-down terminal), or it can be filled with modules (widgets) in a grid fashion. When creating the container, it has to be given a unique swc-key (a string). This key will be used by other instances of swc to request the existing container to slide in or out of the screen.\n";

    const std::string examples =
        "Examples:\n"
        "  toggle existing container  swc <swc-key>\n"
        "  create new container       swc <input-option> [custom-options] <swc-key>\n"
        "  embed htop in a container  swc -b 'urxvt -e htop' swc-top\n"
        "  toggle the htop container  swc swc-top\n";

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
