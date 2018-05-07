QT      += core gui

TARGET = AutoConnect
TEMPLATE = lib
CONFIG += staticlib
CONFIG += release
SOURCES += AutoConnect.cpp
HEADERS += AutoConnect.h

include(../ArkSdk.pri)
system(mkdir -p $$SDKPATH/Package/$$TARGET/$$OUTPUT)
system(cp AutoConnect.h $$SDKPATH/Package/$$TARGET)
