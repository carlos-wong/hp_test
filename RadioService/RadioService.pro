QT += core dbus

TARGET = RadioService
TEMPLATE = lib
CONFIG += staticlib
CONFIG += release
SOURCES += \
    RadioPersistent.cpp

HEADERS += \
    RadioPersistent.h

include(../ArkSdk.pri)
system(mkdir -p $$SDKPATH/Package/$$TARGET/$$OUTPUT)
system(cp RadioPersistent.h $$SDKPATH/Package/$$TARGET)
system(cp RadioService.h $$SDKPATH/Package/$$TARGET)
unix:!macx: LIBS += -L$$SDKPATH/Package/ArkProtocolWrapper/$$OUTPUT -lArkProtocolWrapper
