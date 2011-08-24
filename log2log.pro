#-------------------------------------------------
#
# Project created by QtCreator 2011-08-06T10:30:40
#
#-------------------------------------------------

QT       += core gui xml

TARGET = log2log
TEMPLATE = app


SOURCES += main.cpp \
    log2log.cpp \
    helper.cpp \
    conversion.cpp \
    formatinfo.cpp \
    formats/stdformat.cpp \
    formats/omegle.cpp

HEADERS  += \
    log2log.h \
    helper.h \
    conversion.h \
    formatinfo.h \
    formats/stdformat.h \
    formats/omegle.h

FORMS    += \
    log2log.ui \
    conversion.ui

RESOURCES += \
    log2log.qrc

TRANSLATIONS += en.ts \
    pt.ts

OTHER_FILES += \
    resources/formats.xml
