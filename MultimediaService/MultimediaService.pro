QT      += core gui xml dbus

TARGET = MultimediaService
TEMPLATE = lib
CONFIG += staticlib
CONFIG += release
SOURCES += MultimediaService.cpp \
    DeviceWatcher/DiskScanner/DiskScanner.cpp \
    DeviceWatcher/DeviceWatcher.cpp \
    ImagePlayer/ImagePlayer.cpp \
    MusicPlayer/MusicPlayer.cpp \
    VideoPlayer/VideoPlayer.cpp \
    MultimediaPersistent.cpp \
    MultimediaServiceProxy.cpp \
    DeviceWatcher/DiskDetach/DiskDetach.cpp \
    DeviceWatcher/DiskDeviceWatcher/DiskDeviceWatcher.cpp \
    MusicPlayer/MusicInformation/MusicInformation.cpp
HEADERS += MultimediaService.h \
    DeviceWatcher/DiskScanner/DiskScanner.h \
    DeviceWatcher/DeviceWatcher.h \
    ImagePlayer/ImagePlayer.h \
    MusicPlayer/MusicPlayer.h \
    VideoPlayer/VideoPlayer.h \
    MultimediaServiceProxy.h \
    MultimediaPersistent.h \
    DeviceWatcher/DiskDetach/DiskDetach.h \
    DeviceWatcher/DiskDeviceWatcher/DiskDeviceWatcher.h \
    MusicPlayer/MusicInformation/MusicInformation.h

include(../ArkSdk.pri)
system(mkdir -p $$SDKPATH/Package/$$TARGET/$$OUTPUT)
system(cp MultimediaPersistent.h $$SDKPATH/Package/$$TARGET)
system(cp MultimediaService.h $$SDKPATH/Package/$$TARGET)
system(cp MultimediaServiceProxy.h $$SDKPATH/Package/$$TARGET)

TAGLIB = ./TagLib/Header
INCLUDEPATH += $$TAGLIB
DEPENDPATH += $$TAGLIB
INCLUDEPATH += $$TAGLIB/toolkit
DEPENDPATH += $$TAGLIB/toolkit
INCLUDEPATH += $$TAGLIB/flac
DEPENDPATH += $$TAGLIB/flac
INCLUDEPATH += $$TAGLIB/ape
DEPENDPATH += $$TAGLIB/ape
INCLUDEPATH += $$TAGLIB/mpeg
DEPENDPATH += $$TAGLIB/mpeg
INCLUDEPATH += $$TAGLIB/mpeg/id3v1
DEPENDPATH += $$TAGLIB/mpeg/id3v1
INCLUDEPATH += $$TAGLIB/mpeg/id3v2
DEPENDPATH += $$TAGLIB/mpeg/id3v2
INCLUDEPATH += $$TAGLIB/mpeg/id3v2/frames
DEPENDPATH += $$TAGLIB/mpeg/id3v2/frames
INCLUDEPATH += $$TAGLIB/ogg
DEPENDPATH += $$TAGLIB/ogg
INCLUDEPATH += $$TAGLIB/ogg/flac
DEPENDPATH += $$TAGLIB/ogg/flac
INCLUDEPATH += $$TAGLIB/ogg/opus
DEPENDPATH += $$TAGLIB/ogg/opus
INCLUDEPATH += $$TAGLIB/ogg/speex
DEPENDPATH += $$TAGLIB/ogg/speex
INCLUDEPATH += $$TAGLIB/ogg/vorbis
DEPENDPATH += $$TAGLIB/ogg/vorbis
INCLUDEPATH += $$TAGLIB/mp4
DEPENDPATH += $$TAGLIB/mp4
