QT       += core gui dbus
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = swc
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

CONFIG += c++11

SOURCES += \
    main.cpp \
    AnimatedContainer.cpp \
    CommandLineParser.cpp \
    Settings.cpp

HEADERS += \
    AnimatedContainer.h \
    CommandLineParser.h \
    cloptions.h \
    helptexts.h \
    Settings.h

LIBS += -lxdo

# Build the manpage after linking
QMAKE_POST_LINK = rm -f swc.1.gz && \
                  help2man --help-option=\'-h full\' -N ./swc -o swc.1 && \
                  gzip swc.1
QMAKE_CLEAN += swc.1.gz

# Add install rules
installbin.path = /usr/bin/
installbin.files = swc

installman.path = /usr/share/man/man1/
installman.files = swc.1.gz

INSTALLS += installbin \
            installman
