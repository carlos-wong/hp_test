#-------------------------------------------------
#
# Project created by QtCreator 2017-01-03T11:53:36
#
#-------------------------------------------------

QT       -= core gui
QT       -= gui

TARGET = ArkProtocolWrapper
TEMPLATE = lib
CONFIG += staticlib
CONFIG += release
SOURCES += ArkProtocolWrapper.cpp

HEADERS += ArkProtocolWrapper.h

include(../ArkSdk.pri)
system(mkdir -p $$SDKPATH/Package/$$TARGET/$$OUTPUT)
system(cp ArkProtocolWrapper.h $$SDKPATH/Package/$$TARGET)
