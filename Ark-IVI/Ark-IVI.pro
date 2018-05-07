QT += core gui xml dbus
TEMPLATE = app
CONFIG += release
TARGET = Launcher

TRANSLATIONS += \
    ./Resources/Languages/Launcher_EN.ts \
    ./Resources/Languages/Launcher_CN.ts

SOURCES += \
    main.cpp \
    UserInterface/MainWidget/HomeWidget/HomeWidget.cpp \
    UserInterface/MainWidget/LinkWidget/CarplayLinkWidget/CarplayLinkWidget.cpp \
    UserInterface/MainWidget/LinkWidget/LinkWidget.cpp \
    UserInterface/MainWidget/DiskWidget/ImageWidget/ImageToolBarWidget/ImageToolBarWidget.cpp \
    UserInterface/MainWidget/DiskWidget/ImageWidget/ImageWidget.cpp \
    UserInterface/MainWidget/DiskWidget/MusicWidget/MusicWidget.cpp \
    UserInterface/MainWidget/DiskWidget/VideoWidget/VideoToolBarWidget/VideoToolBarWidget.cpp \
    UserInterface/MainWidget/DiskWidget/VideoWidget/VideoWidget.cpp \
    UserInterface/MainWidget/StatusBarWidget/StatusBarWidget.cpp \
    UserInterface/MainWidget/MainWidget.cpp \
    UserInterface/MainWidget/DiskWidget/USBDiskWidget/USBDiskWidget.cpp \
    UserInterface/MainWidget/DiskWidget/USBDiskWidget/USBDiskListViewWidget/USBDiskMusicListViewWidget/USBDiskMusicListViewWidget.cpp \
    UserInterface/MainWidget/DiskWidget/USBDiskWidget/USBDiskListViewWidget/USBDiskImageListViewWidget/USBDiskImageListViewWidget.cpp \
    UserInterface/MainWidget/DiskWidget/USBDiskWidget/USBDiskListViewWidget/USBDiskVideoListViewWidget/USBDiskVideoListViewWidget.cpp \
    UserInterface/MainWidget/DiskWidget/USBDiskWidget/USBDiskListViewWidget/USBDiskListViewWidget.cpp \
    UserInterface/MainWidget/DiskWidget/USBDiskWidget/USBDiskToolWidget/USBDiskToolWidget.cpp \
    UserInterface/MainWidget/DiskWidget/SDDiskWidget/SDDiskWidget.cpp \
    UserInterface/MainWidget/DiskWidget/SDDiskWidget/SDDiskListViewWidget/SDDiskImageListViewWidget/SDDiskImageListViewWidget.cpp \
    UserInterface/MainWidget/DiskWidget/SDDiskWidget/SDDiskListViewWidget/SDDiskMusicListViewWidget/SDDiskMusicListViewWidget.cpp \
    UserInterface/MainWidget/DiskWidget/SDDiskWidget/SDDiskListViewWidget/SDDiskVideoListViewWidget/SDDiskVideoListViewWidget.cpp \
    UserInterface/MainWidget/DiskWidget/SDDiskWidget/SDDiskListViewWidget/SDDiskListViewWidget.cpp \
    UserInterface/MainWidget/DiskWidget/SDDiskWidget/SDDiskToolWidget/SDDiskToolWidget.cpp \
    UserInterface/MainWidget/DiskWidget/ImageWidget/PixmapWidget/PixmapWidget.cpp \
    UserInterface/MainWidget/VolumeWidget/VolumeWidget.cpp \
    UserInterface/MainWidget/DiskWidget/MusicWidget/MusicToolWidget/MusicToolWidget.cpp \
    UserInterface/MainWidget/DiskWidget/MultimediaListView.cpp \
    UserInterface/MainWidget/HomeWidget/ViewPaperWidget/ViewPaperWidget.cpp \
    UserInterface/MainWidget/HomeWidget/ViewPaperWidget/ViewPaperTipWidget/ViewPaperTipWidget.cpp \
    UserInterface/MainWidget/HomeWidget/TimeWidget/TimeWidget.cpp \
    UserInterface/MainWidget/HomeWidget/UtilityWidget/UtilityWidget.cpp \
    UserInterface/MainWidget/DiskWidget/MusicWidget/MusicSeekBarWidget/MusicSeekBarWidget.cpp \
    UserInterface/MainWidget/DiskWidget/MusicWidget/MusicInformationWidget/MusicInformationWidget.cpp \
    UserInterface/MainWidget/DiskWidget/VideoWidget/VideoSeekBarWidget/VideoSeekBarWidget.cpp \
    UserInterface/MainWidget/HomeWidget/MusicMiniWidget/MusicMiniWidget.cpp \
    UserInterface/MainWidget/SettingWidget/SettingFactoryWidget/SettingFactoryWidget.cpp \
    UserInterface/MainWidget/SettingWidget/SettingGeneralWidget/SettingGeneralWidget.cpp \
    UserInterface/MainWidget/SettingWidget/SettingStyleWidget/SettingStyleWidget.cpp \
    UserInterface/MainWidget/SettingWidget/SettingToolBarWidget/SettingToolBarWidget.cpp \
    UserInterface/MainWidget/SettingWidget/SettingWidget.cpp \
    UserInterface/MainWidget/SettingWidget/SettingGeneralWidget/SettingGeneralDisplayWidget/SettingGeneralDisplayWidget.cpp \
    UserInterface/MainWidget/SettingWidget/SettingGeneralWidget/SettingGeneralLanguageWidget/SettingGeneralLanguageWidget.cpp \
    UserInterface/MainWidget/SettingWidget/SettingGeneralWidget/SettingGeneralTimeWidget/SettingGeneralTimeWidget.cpp \
    UserInterface/MainWidget/SettingWidget/SettingGeneralWidget/SettingGeneralToolWidget/SettingGeneralToolWidget.cpp \
    UserInterface/MainWidget/SettingWidget/SettingGeneralWidget/SettingGeneralVolumeWidget/SettingGeneralVolumeWidget.cpp \
    UserInterface/MainWidget/SettingWidget/SettingGeneralSlider.cpp \
    UserInterface/MainWidget/SettingWidget/SettingGeneralWidget/SettingGeneralTimeWidget/SettingGeneralTimeColumeWidget.cpp \
    UserInterface/MainWidget/BluetoothWidget/BluetoothWidget.cpp \
    UserInterface/MainWidget/RadioWidget/RadioWidget.cpp \
    UserInterface/MainWidget/RadioWidget/RadioSelectorWidget/RadioSelectorWidget.cpp \
    UserInterface/MainWidget/RadioWidget/RadioUtilityWidget/RadioUtilityWidget.cpp \
    UserInterface/MainWidget/RadioWidget/RadioToolWidget/RadioToolWidget.cpp \
    UserInterface/MainWidget/BluetoothWidget/BluetoothToolWidget/BluetoothToolWidget.cpp \
    UserInterface/MainWidget/BluetoothWidget/BluetoothSettingWidget/BluetoothSettingWidget.cpp \
    UserInterface/MainWidget/BluetoothWidget/BluetoothRecordWidget/BluetoothRecordWidget.cpp \
    UserInterface/MainWidget/BluetoothWidget/BluetoothMusicWidget/BluetoothMusicWidget.cpp \
    UserInterface/MainWidget/BluetoothWidget/BluetoothContactWidget/BluetoothContactWidget.cpp \
    UserInterface/MainWidget/RadioWidget/RadioListViewWidget/RadioListViewWidget.cpp \
    UserInterface/MainWidget/BluetoothWidget/BluetoothDialWidget/BluetoothDialWidget.cpp \
    UserInterface/MainWidget/SettingWidget/SettingStyleWidget/SettingStyleWallpaperWidget/SettingStyleWallpaperWidget.cpp \
    UserInterface/MainWidget/SettingWidget/SettingStyleWidget/SettingStyleToolWidget/SettingStyleToolWidget.cpp \
    UserInterface/MainWidget/BluetoothWidget/BluetoothSettingWidget/BluetoothSettingStatusWidget/BluetoothSettingStatusWidget.cpp \
    UserInterface/MainWidget/BluetoothWidget/BluetoothSettingWidget/BluetoothSettingInformationWidget/BluetoothSettingInformationWidget.cpp \
    BusinessLogic/Audio.cpp \
    BusinessLogic/BusinessLogicUtility.cpp \
    BusinessLogic/CarlifeLinkProxy.cpp \
    BusinessLogic/CarplayLinkProxy.cpp \
    BusinessLogic/Link.cpp \
    BusinessLogic/Multimedia.cpp \
    BusinessLogic/Radio.cpp \
    BusinessLogic/Setting.cpp \
    BusinessLogic/Widget.cpp \
    BusinessLogic/Bluetooth.cpp \
    UserInterface/Launcher.cpp \
    UserInterface/CalibrateWidget/CalibrateWidget.cpp \
    UserInterface/ReversingWidget/ReversingWidget.cpp \
    UserInterface/MainWidget/HomeWidget/RadioMiniWidget/RadioMiniWidget.cpp \
    UserInterface/MainWidget/LinkWidget/CarlifeLinkWidget/CarlifeLinkWidget.cpp \
    UserInterface/IdleWidget/IdleWidget.cpp \
    UserInterface/MainWidget/SteeringWidget/SteeringWidget.cpp \
    UserInterface/MainWidget/SettingWidget/SettingSoundWidget/SettingEqualizerWidget/SettingEqualizerWidget.cpp \
    UserInterface/MainWidget/SettingWidget/SettingSoundWidget/SettingSoundSoundWidget/SettingSoundSoundWidget.cpp \
    UserInterface/MainWidget/SettingWidget/SettingSoundWidget/SettingSoundToolWidget/SettingSoundToolWidget.cpp \
    UserInterface/MainWidget/SettingWidget/SettingSoundWidget/SettingSoundWidget.cpp \
    UserInterface/MainWidget/HomeWidget/BrandMiniWidget/BrandMiniWidget.cpp \
    UserInterface/MainWidget/SettingWidget/SettingFactoryWidget/SettingFactoryToolWidget/SettingFactoryToolWidget.cpp \
    UserInterface/MainWidget/SettingWidget/SettingMessageDialog.cpp \
    UserInterface/MainWidget/BluetoothWidget/BluetoothMessageDialog.cpp \
    UserInterface/MainWidget/BluetoothWidget/BluetoothSettingWidget/BluetoothSettingPairedListWidget/BluetoothSettingPairedListWidget.cpp \
    UserInterface/MainWidget/HomeWidget/BluetoothMusicMiniWidget/BluetoothMusicMiniWidget.cpp \
    UserInterface/MainWidget/BluetoothWidget/BluetoothSettingWidget/BluetoothSettingPairedListWidget/BluetoothSettingPairedListItem/BluetoothSettingPairedListItem.cpp \
    UserInterface/MainWidget/CameraWidget/CameraWidget.cpp \
    BusinessLogic/AutoLinkProxy.cpp \
    UserInterface/MainWidget/LinkWidget/AutoLinkWidget/AutoLinkWidget.cpp \
    UserInterface/MainWidget/AUXWidget/AUXWidget.cpp \
    UserInterface/MainWidget/BluetoothWidget/BluetoothSettingWidget/BluetoothSettingAlphabetPadWidget/BluetoothSettingAlphabetPadWidget.cpp \
    UserInterface/MainWidget/BluetoothWidget/BluetoothSettingWidget/BluetoothSettingNumberPadWidget/BluetoothSettingNumberPadWidget.cpp \
    UserInterface/MainWidget/SettingWidget/SettingGeneralWidget/SettingGeneralVersionWidget/SettingGeneralVersionWidget.cpp \
    UserInterface/MainWidget/SettingWidget/SettingFactoryWidget/SettingFactoryNumberPadWidget/SettingFactoryNumberPadWidget.cpp \
    UserInterface/MainWidget/SettingWidget/SettingFactoryWidget/SettingFactoryLinkWidget/SettingFactoryLinkWidget.cpp \
    UserInterface/MainWidget/SettingWidget/SettingFactoryWidget/SettingFactoryLogoWidget/SettingFactoryLogoWidget.cpp

HEADERS += \
    UserInterface/MainWidget/HomeWidget/HomeWidget.h \
    UserInterface/MainWidget/LinkWidget/CarplayLinkWidget/CarplayLinkWidget.h \
    UserInterface/MainWidget/LinkWidget/LinkWidget.h \
    UserInterface/MainWidget/DiskWidget/ImageWidget/ImageToolBarWidget/ImageToolBarWidget.h \
    UserInterface/MainWidget/DiskWidget/ImageWidget/ImageWidget.h \
    UserInterface/MainWidget/DiskWidget/MusicWidget/MusicWidget.h \
    UserInterface/MainWidget/DiskWidget/VideoWidget/VideoToolBarWidget/VideoToolBarWidget.h \
    UserInterface/MainWidget/DiskWidget/VideoWidget/VideoWidget.h \
    UserInterface/MainWidget/StatusBarWidget/StatusBarWidget.h \
    UserInterface/MainWidget/MainWidget.h \
    UserInterface/MainWidget/DiskWidget/USBDiskWidget/USBDiskWidget.h \
    UserInterface/MainWidget/DiskWidget/USBDiskWidget/USBDiskListViewWidget/USBDiskMusicListViewWidget/USBDiskMusicListViewWidget.h \
    UserInterface/MainWidget/DiskWidget/USBDiskWidget/USBDiskListViewWidget/USBDiskImageListViewWidget/USBDiskImageListViewWidget.h \
    UserInterface/MainWidget/DiskWidget/USBDiskWidget/USBDiskListViewWidget/USBDiskVideoListViewWidget/USBDiskVideoListViewWidget.h \
    UserInterface/MainWidget/DiskWidget/USBDiskWidget/USBDiskListViewWidget/USBDiskListViewWidget.h \
    UserInterface/MainWidget/DiskWidget/USBDiskWidget/USBDiskToolWidget/USBDiskToolWidget.h \
    UserInterface/MainWidget/DiskWidget/SDDiskWidget/SDDiskWidget.h \
    UserInterface/MainWidget/DiskWidget/SDDiskWidget/SDDiskListViewWidget/SDDiskImageListViewWidget/SDDiskImageListViewWidget.h \
    UserInterface/MainWidget/DiskWidget/SDDiskWidget/SDDiskListViewWidget/SDDiskMusicListViewWidget/SDDiskMusicListViewWidget.h \
    UserInterface/MainWidget/DiskWidget/SDDiskWidget/SDDiskListViewWidget/SDDiskVideoListViewWidget/SDDiskVideoListViewWidget.h \
    UserInterface/MainWidget/DiskWidget/SDDiskWidget/SDDiskListViewWidget/SDDiskListViewWidget.h \
    UserInterface/MainWidget/DiskWidget/SDDiskWidget/SDDiskToolWidget/SDDiskToolWidget.h \
    UserInterface/MainWidget/DiskWidget/ImageWidget/PixmapWidget/PixmapWidget.h \
    UserInterface/MainWidget/VolumeWidget/VolumeWidget.h \
    UserInterface/MainWidget/DiskWidget/MusicWidget/MusicToolWidget/MusicToolWidget.h \
    UserInterface/MainWidget/DiskWidget/MultimediaListView.h \
    UserInterface/MainWidget/HomeWidget/ViewPaperWidget/ViewPaperWidget.h \
    UserInterface/MainWidget/HomeWidget/ViewPaperWidget/ViewPaperTipWidget/ViewPaperTipWidget.h \
    UserInterface/MainWidget/HomeWidget/TimeWidget/TimeWidget.h \
    UserInterface/MainWidget/HomeWidget/UtilityWidget/UtilityWidget.h \
    UserInterface/MainWidget/DiskWidget/MusicWidget/MusicSeekBarWidget/MusicSeekBarWidget.h \
    UserInterface/MainWidget/DiskWidget/MusicWidget/MusicInformationWidget/MusicInformationWidget.h \
    UserInterface/MainWidget/DiskWidget/VideoWidget/VideoSeekBarWidget/VideoSeekBarWidget.h \
    UserInterface/MainWidget/HomeWidget/MusicMiniWidget/MusicMiniWidget.h \
    UserInterface/MainWidget/SettingWidget/SettingFactoryWidget/SettingFactoryWidget.h \
    UserInterface/MainWidget/SettingWidget/SettingGeneralWidget/SettingGeneralWidget.h \
    UserInterface/MainWidget/SettingWidget/SettingStyleWidget/SettingStyleWidget.h \
    UserInterface/MainWidget/SettingWidget/SettingToolBarWidget/SettingToolBarWidget.h \
    UserInterface/MainWidget/SettingWidget/SettingWidget.h \
    UserInterface/MainWidget/SettingWidget/SettingGeneralWidget/SettingGeneralDisplayWidget/SettingGeneralDisplayWidget.h \
    UserInterface/MainWidget/SettingWidget/SettingGeneralWidget/SettingGeneralLanguageWidget/SettingGeneralLanguageWidget.h \
    UserInterface/MainWidget/SettingWidget/SettingGeneralWidget/SettingGeneralTimeWidget/SettingGeneralTimeWidget.h \
    UserInterface/MainWidget/SettingWidget/SettingGeneralWidget/SettingGeneralToolWidget/SettingGeneralToolWidget.h \
    UserInterface/MainWidget/SettingWidget/SettingGeneralWidget/SettingGeneralVolumeWidget/SettingGeneralVolumeWidget.h \
    UserInterface/MainWidget/SettingWidget/SettingGeneralSlider.h \
    UserInterface/MainWidget/SettingWidget/SettingGeneralWidget/SettingGeneralTimeWidget/SettingGeneralTimeColumeWidget.h \
    UserInterface/MainWidget/BluetoothWidget/BluetoothWidget.h \
    UserInterface/MainWidget/RadioWidget/RadioWidget.h \
    UserInterface/MainWidget/RadioWidget/RadioSelectorWidget/RadioSelectorWidget.h \
    UserInterface/MainWidget/RadioWidget/RadioUtilityWidget/RadioUtilityWidget.h \
    UserInterface/MainWidget/RadioWidget/RadioToolWidget/RadioToolWidget.h \
    UserInterface/MainWidget/BluetoothWidget/BluetoothToolWidget/BluetoothToolWidget.h \
    UserInterface/MainWidget/BluetoothWidget/BluetoothSettingWidget/BluetoothSettingWidget.h \
    UserInterface/MainWidget/BluetoothWidget/BluetoothRecordWidget/BluetoothRecordWidget.h \
    UserInterface/MainWidget/BluetoothWidget/BluetoothMusicWidget/BluetoothMusicWidget.h \
    UserInterface/MainWidget/BluetoothWidget/BluetoothContactWidget/BluetoothContactWidget.h \
    UserInterface/MainWidget/RadioWidget/RadioListViewWidget/RadioListViewWidget.h \
    UserInterface/MainWidget/BluetoothWidget/BluetoothDialWidget/BluetoothDialWidget.h \
    UserInterface/MainWidget/SettingWidget/SettingStyleWidget/SettingStyleWallpaperWidget/SettingStyleWallpaperWidget.h \
    UserInterface/MainWidget/SettingWidget/SettingStyleWidget/SettingStyleToolWidget/SettingStyleToolWidget.h \
    UserInterface/MainWidget/BluetoothWidget/BluetoothSettingWidget/BluetoothSettingStatusWidget/BluetoothSettingStatusWidget.h \
    UserInterface/MainWidget/BluetoothWidget/BluetoothSettingWidget/BluetoothSettingInformationWidget/BluetoothSettingInformationWidget.h \
    BusinessLogic/Audio.h \
    BusinessLogic/BusinessLogicUtility.h \
    BusinessLogic/CarlifeLinkProxy.h \
    BusinessLogic/CarplayLinkProxy.h \
    BusinessLogic/Link.h \
    BusinessLogic/Multimedia.h \
    BusinessLogic/Radio.h \
    BusinessLogic/Setting.h \
    BusinessLogic/Widget.h \
    BusinessLogic/Bluetooth.h \
    UserInterface/Launcher.h \
    UserInterface/CalibrateWidget/CalibrateWidget.h \
    UserInterface/ReversingWidget/ReversingWidget.h \
    UserInterface/MainWidget/HomeWidget/RadioMiniWidget/RadioMiniWidget.h \
    UserInterface/MainWidget/LinkWidget/CarlifeLinkWidget/CarlifeLinkWidget.h \
    UserInterface/IdleWidget/IdleWidget.h \
    UserInterface/MainWidget/AUXWidget/AUXWidget.h \
    UserInterface/MainWidget/SteeringWidget/SteeringWidget.h \
    UserInterface/MainWidget/SettingWidget/SettingSoundWidget/SettingEqualizerWidget/SettingEqualizerWidget.h \
    UserInterface/MainWidget/SettingWidget/SettingSoundWidget/SettingSoundSoundWidget/SettingSoundSoundWidget.h \
    UserInterface/MainWidget/SettingWidget/SettingSoundWidget/SettingSoundToolWidget/SettingSoundToolWidget.h \
    UserInterface/MainWidget/SettingWidget/SettingSoundWidget/SettingSoundWidget.h \
    UserInterface/MainWidget/HomeWidget/BrandMiniWidget/BrandMiniWidget.h \
    UserInterface/MainWidget/SettingWidget/SettingFactoryWidget/SettingFactoryToolWidget/SettingFactoryToolWidget.h \
    UserInterface/MainWidget/SettingWidget/SettingMessageDialog.h \
    UserInterface/MainWidget/BluetoothWidget/BluetoothMessageDialog.h \
    UserInterface/MainWidget/BluetoothWidget/BluetoothSettingWidget/BluetoothSettingPairedListWidget/BluetoothSettingPairedListWidget.h \
    UserInterface/MainWidget/HomeWidget/BluetoothMusicMiniWidget/BluetoothMusicMiniWidget.h \
    UserInterface/MainWidget/BluetoothWidget/BluetoothSettingWidget/BluetoothSettingPairedListWidget/BluetoothSettingPairedListItem/BluetoothSettingPairedListItem.h \
    UserInterface/MainWidget/CameraWidget/CameraWidget.h \
    BusinessLogic/AutoLinkProxy.h \
    UserInterface/MainWidget/LinkWidget/AutoLinkWidget/AutoLinkWidget.h \
    UserInterface/MainWidget/BluetoothWidget/BluetoothSettingWidget/BluetoothSettingAlphabetPadWidget/BluetoothSettingAlphabetPadWidget.h \
    UserInterface/MainWidget/BluetoothWidget/BluetoothSettingWidget/BluetoothSettingNumberPadWidget/BluetoothSettingNumberPadWidget.h \
    UserInterface/MainWidget/SettingWidget/SettingGeneralWidget/SettingGeneralVersionWidget/SettingGeneralVersionWidget.h \
    UserInterface/MainWidget/SettingWidget/SettingFactoryWidget/SettingFactoryNumberPadWidget/SettingFactoryNumberPadWidget.h \
    UserInterface/MainWidget/SettingWidget/SettingFactoryWidget/SettingFactoryLinkWidget/SettingFactoryLinkWidget.h \
    UserInterface/MainWidget/SettingWidget/SettingFactoryWidget/SettingFactoryLogoWidget/SettingFactoryLogoWidget.h

include(../ArkSdk.pri)

MOC_DIR = $$SDKPATH/Package/Ark-IVI/$$OUTPUT
OBJECTS_DIR = $$SDKPATH/Package/Ark-IVI/$$OUTPUT
RCC_DIR = $$SDKPATH/Package/Ark-IVI/$$OUTPUT
DESTDIR = $$SDKPATH/Package/Ark-IVI/$$OUTPUT

unix:!macx: LIBS += -L$$SDKPATH/Package/UserInterface/$$OUTPUT -lUserInterface
unix:!macx: LIBS += -L$$SDKPATH/Package/ArkApplication/$$OUTPUT -lArkApplication
unix:!macx: LIBS += -L$$SDKPATH/Package/AutoConnect/$$OUTPUT -lAutoConnect
unix:!macx: LIBS += -L$$SDKPATH/Package/DbusService/$$OUTPUT -lDbusService
unix:!macx: LIBS += -L$$SDKPATH/Package/AudioService/$$OUTPUT -lAudioService
unix:!macx: LIBS += -L$$SDKPATH/Package/SettingService/$$OUTPUT -lSettingService
unix:!macx: LIBS += -L$$SDKPATH/Package/MultimediaService/$$OUTPUT -lMultimediaService
unix:!macx: LIBS += -L$$SDKPATH/Package/RadioService/$$OUTPUT -lRadioService
unix:!macx: LIBS += -L$$SDKPATH/Package/BluetoothService/$$OUTPUT -lBluetoothService
unix:!macx: LIBS += -L$$SDKPATH/Package/Utility/$$OUTPUT -lUtility
unix:!macx: LIBS += -L$$SDKPATH/Package/RunnableThread/$$OUTPUT -lRunnableThread

TAGLIB = $$SDKPATH/ArkMicro/Header/TagLib
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


ALSA = $$SDKPATH/Alsa/Header
INCLUDEPATH += $$ALSA
DEPENDPATH += $$ALSA

if(contains(DEFINES,DESKTOP_AMD64)){
    message("compile by gcc")
    unix:!macx: LIBS += -L$$SDKPATH/Package/ArkProtocolWrapper/$$OUTPUT -lArkProtocolWrapper
}else{
    message("compile by arm-none-linux-gnueabi-gcc")
    unix:!macx: LIBS += -L$$SDKPATH/Alsa/Library -lasound
    unix:!macx: LIBS += -ldl
    unix:!macx: LIBS += -L$$SDKPATH/ArkMicro/Library -larkcmn
    unix:!macx: LIBS += -L$$SDKPATH/ArkMicro/Library -lDetach
    unix:!macx: LIBS += -L$$SDKPATH/ArkMicro/Library -lDetachClient
    unix:!macx: LIBS += -L$$SDKPATH/ArkNaviProtocol/Library -lArkNaviProtocol
    unix:!macx: LIBS += -L$$SDKPATH/TagLib/Library -lQtConvert
    unix:!macx: LIBS += -L$$SDKPATH/TagLib/Library -lConvert
    unix:!macx: LIBS += -L$$SDKPATH/TagLib/Library -ltag
    unix:!macx: LIBS += -L$$SDKPATH/Package/ArkProtocolWrapper/$$OUTPUT -lArkProtocolWrapper
}

RESOURCES += \
    Resources/Languages.qrc
