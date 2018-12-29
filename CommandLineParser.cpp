#include "CommandLineParser.h"
#include "AnimatedContainer.h"
#include "cloptions.h"

CommandLineParser::CommandLineParser()
{
    parser.setApplicationDescription("Embed windows easily and animate them");
    parser.addHelpOption();

    parser.addOption(cloptions::className);
}

void CommandLineParser::process(const QCoreApplication &app)
{
    bool foundOption = false;

    parser.process(app);
    if (!foundOption && parser.isSet(cloptions::className)) {
        container = new AnimatedContainer(parser.value(cloptions::className));
        foundOption = true;
    }

    // Exit if no option was found
    if (!foundOption) {
        parser.showHelp();
    }

//    container->setWindowFlag(Qt::X11BypassWindowManagerHint);
    container->setWindowFlag(Qt::WindowStaysOnTopHint);
//    container->setWindowFlag(Qt::Desktop);
//    container->setWindowFlag(Qt::BypassWindowManagerHint);
    container->show();
}
