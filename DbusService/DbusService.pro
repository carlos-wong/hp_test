QT += core gui dbus

TARGET = DbusService
TEMPLATE = lib
CONFIG += staticlib
CONFIG += release
SOURCES += \
    DbusService.cpp

HEADERS += \
    DbusService.h

include(../ArkSdk.pri)
system(mkdir -p $$SDKPATH/Package/$$TARGET/$$OUTPUT)
system(cp DbusService.h $$SDKPATH/Package/$$TARGET)
