#-------------------------------------------------
#
# Project created by QtCreator 2011-08-06T10:30:40
#
#-------------------------------------------------

QT       += core gui xml network

TARGET = log2log
TEMPLATE = app

ICON = images/etc/favicon.ico
win32:RC_FILE += log2log.rc

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
    formats/pidgin.cpp \
    formats/meebo.cpp \
    json.cpp \
    formats/stdjson.cpp \
    conversionconsole.cpp \
    formats/trillian.cpp \
    formats/wlm.cpp \
    formats/aim.cpp \
    formats/skype.cpp \
    formats/meeboconnect.cpp

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
    formats/pidgin.h \
    formats/meebo.h \
    json.h \
    formats/stdjson.h \
    conversionconsole.h \
    formats/trillian.h \
    formats/wlm.h \
    formats/aim.h \
    formats/skype.h \
    formats/meeboconnect.h

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

unix {
    isEmpty(PREFIX) {
        PREFIX = /usr
    }
    binaries.path = $$PREFIX/bin
    binaries.files = $$TARGET
    INSTALLS += binaries
}
