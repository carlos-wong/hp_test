#-------------------------------------------------
#
# Project created by QtCreator 2017-01-03T11:53:36
#
#-------------------------------------------------

QT       += core gui

TARGET = Utility
TEMPLATE = lib
CONFIG += staticlib
CONFIG += release
SOURCES += Utility.cpp

HEADERS += Utility.h

include(../ArkSdk.pri)
system(mkdir -p $$SDKPATH/Package/$$TARGET/$$OUTPUT)
system(cp Utility.h $$SDKPATH/Package/$$TARGET)
