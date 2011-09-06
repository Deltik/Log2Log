#-------------------------------------------------
#
# Project created by QtCreator 2011-08-06T10:30:40
#
#-------------------------------------------------

QT       += core gui xml network

TARGET = log2log
TEMPLATE = app


SOURCES += main.cpp \
    log2log.cpp \
    helper.cpp \
    formatinfo.cpp \
    formats/stdformat.cpp \
    formats/omegle.cpp \
    about.cpp \
    conversion.cpp \
    formats/stdconverter.cpp \
    update.cpp \
    api.cpp \
    formats/pidgin.cpp

HEADERS  += \
    log2log.h \
    helper.h \
    formatinfo.h \
    formats/stdformat.h \
    formats/omegle.h \
    about.h \
    conversion.h \
    formats/stdconverter.h \
    update.h \
    api.h \
    formats/pidgin.h

FORMS    += \
    log2log.ui \
    about.ui \
    update.ui

RESOURCES += \
    log2log.qrc

TRANSLATIONS += en.ts \
    pt.ts

OTHER_FILES += \
    resources/formats.xml
