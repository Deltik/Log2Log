#-------------------------------------------------
#
# Project created by QtCreator 2011-08-06T10:30:40
#
#-------------------------------------------------

QT       += core gui declarative xml network

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
    formats/devconverter.cpp \
    formats/imo.cpp \
    formats/meebofarewell.cpp \
    formats/digsby.cpp

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
    formats/devconverter.h \
    formats/imo.h \
    formats/meebofarewell.h \
    resources/timezonemap.h \
    formats/digsby.h

FORMS    += \
    log2log.ui \
    about.ui \
    update.ui

RESOURCES += \
    log2log.qrc

TRANSLATIONS += en.ts \
    pt.ts

OTHER_FILES += \
    resources/formats.xml \
    images/etc/Arrow.png \
    images/etc/blank.png \
    images/etc/cancel.png \
    images/etc/Deltik.png \
    images/etc/Log2Log.png \
    images/services/Adium.png \
    images/services/AIM.png \
    images/services/Deltik.png \
    images/services/Digsby.png \
    images/services/Empathy.png \
    images/services/Facebook.png \
    images/services/GChat.png \
    images/services/GVoice.png \
    images/services/imo.png \
    images/services/JSON.png \
    images/services/Kopete.png \
    images/services/Log2Log.png \
    images/services/Meebo.png \
    images/services/MeeboConnect.png \
    images/services/Minecraft.png \
    images/services/Omegle.png \
    images/services/Pidgin.png \
    images/services/Skype.png \
    images/services/Trillian.png \
    images/services/WLM.png \
    resources/MessageLog.xsl \
    COPYING.txt \
    LICENSE.txt \
    README.txt

unix {
    isEmpty(PREFIX) {
        PREFIX = /usr
    }
    binaries.path = $$PREFIX/bin
    binaries.files = $$TARGET
    INSTALLS += binaries
}
