#include "CommandLineParser.h"
#include <QApplication>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    QApplication::setApplicationName("SWC");

    CommandLineParser parser;
    parser.process(app);

    return app.exec();
}
