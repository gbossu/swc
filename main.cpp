#include "CommandLineParser.h"
#include <QApplication>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    QApplication::setApplicationName("SWC");

    CommandLineParser parser;
    parser.process(app);

    if (parser.isOwningContainer())
        return app.exec();
    else
        return 0;
}
