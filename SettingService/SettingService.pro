QT += core dbus

TARGET = SettingService
TEMPLATE = lib
CONFIG += staticlib
CONFIG += release
SOURCES += \
    SettingService.cpp \
    SettingPersistent.cpp

HEADERS += \
    SettingService.h \
    SettingPersistent.h

include(../ArkSdk.pri)
system(mkdir -p $$SDKPATH/Package/$$TARGET/$$OUTPUT)
system(cp SettingPersistent.h $$SDKPATH/Package/$$TARGET)
system(cp SettingService.h $$SDKPATH/Package/$$TARGET)
