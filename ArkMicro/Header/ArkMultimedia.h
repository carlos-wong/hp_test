#ifndef ARKMULTIMEDIA_H
#define ARKMULTIMEDIA_H

enum ArkMultimediaType {
    AMT_Undefine = -1,
    AMT_SDDisk,
    AMT_USBDisk,
    AMT_SDMusic,
    AMT_SDVideo,
    AMT_USBMusic,
    AMT_USBVideo,
};

#define AMS_DiskStatus  AMS_DiskEmpty,   \
                        AMS_DiskUnsupport, \
                        AMS_DiskBusy,   \
                        AMS_DiskReady,  \
                        AMS_DiskRemove

#define AMS_AVStatus    AMS_AVStart, \
                        AMS_AVUnsupport, \
                        AMS_AVStop,  \
                        AMS_AVPause, \
                        AMS_AVPlay,  \
                        AMS_AVExit

enum ArkMultimediaStatus {
    AMS_Undefine = -1,
    AMS_DiskStatus,
    AMS_AVStatus
};

struct ArkMultimedia {
    enum ArkMultimediaType type;
    enum ArkMultimediaStatus status;
};

#endif // ARKMULTIMEDIA_H
