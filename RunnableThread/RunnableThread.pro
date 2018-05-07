#-------------------------------------------------
#
# Project created by QtCreator 2017-01-03T14:11:00
#
#-------------------------------------------------

QT       += core gui

TARGET = RunnableThread
TEMPLATE = lib
CONFIG += staticlib
CONFIG += release
SOURCES += RunnableThread.cpp
HEADERS += RunnableThread.h

include(../ArkSdk.pri)
system(mkdir -p $$SDKPATH/Package/$$TARGET/$$OUTPUT)
system(cp RunnableThread.h $$SDKPATH/Package/$$TARGET)
