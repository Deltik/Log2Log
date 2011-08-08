#-------------------------------------------------
#
# Project created by QtCreator 2011-08-06T10:30:40
#
#-------------------------------------------------

QT       += core gui

TARGET = log2log
TEMPLATE = app


SOURCES += main.cpp \
    log2log.cpp \
    helpers.cpp \
    conversion.cpp \
    xmltest.cpp

HEADERS  += \
    log2log.h \
    helpers.h \
    conversion.h \
    xmltest.h

FORMS    += \
    log2log.ui \
    conversion.ui \
    xmltest.ui

RESOURCES += \
    log2log.qrc

TRANSLATIONS += en.ts \
    pt.ts

OTHER_FILES += \
    resources/formats.xml
