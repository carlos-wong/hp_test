QT += core

TARGET = BluetoothService
TEMPLATE = lib
CONFIG += staticlib
CONFIG += release
SOURCES += \
    BluetoothPersistent.cpp

HEADERS += \
    BluetoothPersistent.h

include(../ArkSdk.pri)
system(mkdir -p $$SDKPATH/Package/$$TARGET/$$OUTPUT)
system(cp BluetoothPersistent.h $$SDKPATH/Package/$$TARGET)
system(cp BluetoothService.h $$SDKPATH/Package/$$TARGET)
unix:!macx: LIBS += -L$$SDKPATH/Package/ArkProtocolWrapper/$$OUTPUT -lArkProtocolWrapper
