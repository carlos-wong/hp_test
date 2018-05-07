#-------------------------------------------------
#
# Project created by QtCreator 2017-01-03T11:53:36
#
#-------------------------------------------------

QT       += core gui

TARGET = UserInterface
TEMPLATE = lib
CONFIG += staticlib
CONFIG += release
include(../ArkSdk.pri)
system(mkdir -p $$SDKPATH/Package/$$TARGET/$$OUTPUT)
system(cp BmpButton.h $$SDKPATH/Package/$$TARGET)
system(cp BmpWidget.h $$SDKPATH/Package/$$TARGET)
system(cp CustomItemDelegate.h $$SDKPATH/Package/$$TARGET)
system(cp CustomListView.h $$SDKPATH/Package/$$TARGET)
system(cp CustomScrollBar.h $$SDKPATH/Package/$$TARGET)
system(cp MessageBox.h $$SDKPATH/Package/$$TARGET)
system(cp Slider.h $$SDKPATH/Package/$$TARGET)
system(cp TextWidget.h $$SDKPATH/Package/$$TARGET)
system(cp UserInterfaceUtility.h $$SDKPATH/Package/$$TARGET)

HEADERS += \
    BmpButton.h \
    BmpWidget.h \
    CustomItemDelegate.h \
    CustomListView.h \
    CustomScrollBar.h \
    MessageBox.h \
    Slider.h \
    TextWidget.h \
    UserInterfaceUtility.h

SOURCES += \
    BmpButton.cpp \
    BmpWidget.cpp \
    CustomItemDelegate.cpp \
    CustomListView.cpp \
    CustomScrollBar.cpp \
    MessageBox.cpp \
    Slider.cpp \
    TextWidget.cpp \
    UserInterfaceUtility.cpp
