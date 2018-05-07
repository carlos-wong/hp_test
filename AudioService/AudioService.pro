QT += core gui dbus

TARGET = AudioService
TEMPLATE = lib
CONFIG += staticlib
CONFIG += release
SOURCES += \
    AudioService.cpp \
    AudioServiceProxy.cpp \
    audiocontrol.cpp \
    AudioPersistent.cpp

HEADERS += \
    AudioService.h \
    AudioServiceProxy.h \
    audiocontrol.h \
    AudioPersistent.h

include(../ArkSdk.pri)
system(mkdir -p $$SDKPATH/Package/$$TARGET/$$OUTPUT)
system(cp AudioService.h $$SDKPATH/Package/$$TARGET)
system(cp AudioServiceProxy.h $$SDKPATH/Package/$$TARGET)
system(cp AudioPersistent.h $$SDKPATH/Package/$$TARGET)

#if (contains(DEFINES, $$COMPILER)) {
INCLUDEPATH += ./Alsa/Header
DEPENDPATH += ./Alsa/Header
unix:!macx: LIBS += -L./Alsa/Library -lasound
#}
