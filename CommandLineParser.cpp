#include "CommandLineParser.h"
#include "AnimatedContainer.h"
#include "cloptions.h"

CommandLineParser::CommandLineParser() :
    container(nullptr), owningContainer(false)
{
    parser.setApplicationDescription("Embed windows easily and animate them");
    parser.addPositionalArgument("swc-key",
                                 "The unique swc-key this container should use");
    parser.addHelpOption();

    parser.addOption(cloptions::className);
}

CommandLineParser::~CommandLineParser()
{
}

void CommandLineParser::process(const QCoreApplication &app)
{
    bool foundOption = false;
    parser.process(app);

    // Exit if no swc-key was given
    if (parser.positionalArguments().size() != 1)
        parser.showHelp();
    swcKey = "swc-" + parser.positionalArguments().at(0);

    // Check the options for creating a container
    if (!foundOption && parser.isSet(cloptions::className)) {
        container = new AnimatedContainer(parser.value(cloptions::className));
        owningContainer = true;
        foundOption = true;
    }

    // Exit if no option was found to create a container
    if (!foundOption) {
        parser.showHelp();
    }

    if (owningContainer) {
    //    container->setWindowFlag(Qt::X11BypassWindowManagerHint);
        container->setWindowFlag(Qt::WindowStaysOnTopHint);
    //    container->setWindowFlag(Qt::Desktop);
    //    container->setWindowFlag(Qt::BypassWindowManagerHint);
        container->show();
        container->animate();
    }
}

bool CommandLineParser::isOwningContainer() const
{
    return owningContainer;
}
