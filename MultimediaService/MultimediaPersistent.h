#ifndef MULTIMEDIAPERSISTENT_H
#define MULTIMEDIAPERSISTENT_H

#include <QtGlobal>

enum DeviceWatcherType {
    DWT_Undefine = -1,
    DWT_SDDisk = 0,
    DWT_USBDisk = 1,
};
#define DeviceWatcherType int

enum DeviceWatcherStatus {
    DWS_Undefine = -1,
    DWS_Empty = 0,
    DWS_Unsupport = 1,
    DWS_Busy = 2,
    DWS_Ready = 3,
    DWS_Remove = 4,
};
#define DeviceWatcherStatus int

enum MusicPlayerPlayMode {
    MPPM_AllRepeat = 0,
    MPPM_Shuffle = 1,
    MPPM_RepeatOnce = 2,
};
#define MusicPlayerPlayMode int

enum MusicPlayerPlayStatus {
    MPPS_Undefine = -1,
    MPPS_Start = 0,
    MPPS_Unsupport = 1,
    MPPS_Stop = 2,
    MPPS_Pause = 3,
    MPPS_Play = 4,
    MPPS_Exit = 5,
};
#define MusicPlayerPlayStatus int

enum VideoPlayerPlayStatus {
    VPPS_Undefine = -1,
    VPPS_Start = 0,
    VPPS_Unsupport = 1,
    VPPS_Stop = 2,
    VPPS_Pause = 3,
    VPPS_SuspendToggle = 4,
    VPPS_Play = 5,
    VPPS_Exit = 6,
};
#define VideoPlayerPlayStatus int

enum VideoPlayerPlayMode {
    VPPM_AllRepeat = 0,
    VPPM_Shuffle = 1,
    VPPM_RepeatOnce = 2,
};
#define VideoPlayerPlayMode int

enum MultimediaType {
    MT_Undefine = -1,
    MT_Idle = 0,
    MT_Music = 1,
    MT_Image = 2,
    MT_Video = 3,
};
#define MultimediaType int

class MultimediaPersistent
{
    Q_DISABLE_COPY(MultimediaPersistent)
public:
    static int getMultimediaType(const int deviceWatcherType, const bool reload = true);
//    static int getMultimediaType(const DeviceWatcherType distType, const MultimediaType multimediaType, const bool reload = true);
    static int getMusicMillesmial(const int type, const bool reload = true);
//    static int getMusicMillesmial(const DeviceWatcherType type, const bool reload = true);
    static QString getMusicPathInfo(const int type, const bool reload = true);
//    static QString getMusicPath(const DeviceWatcherType type, const bool reload = true);
    static QString getImagePathInfo(const int type, const bool reload = true);
//    static QString getImagePath(const DeviceWatcherType type, const bool reload = true);
    static int getVideoMillesmial(const int type, const bool reload = true);
//    static int getVideoMillesmial(const DeviceWatcherType type, const bool reload = true);
    static QString getVideoPathInfo(const int type, const bool reload = true);
//    static QString getVideoPathInfo(const DeviceWatcherType type, const bool reload = true);
protected:
    friend class MultimediaService;
    friend class MusicPlayerPrivate;
    friend class ImagePlayerPrivate;
    friend class VideoPlayerPrivate;
    static void setMultimediaType(const int deviceWatcherType, const int multimediaType);
//    static void setUSBMultimediaType(const DeviceWatcherType distType, const MultimediaType multimediaType);
    static void setMusicMillesmial(const int type, const int millesmial);
//    static void setMusicMillesmial(const DeviceWatcherType type, const int millesmial);
    static void setMusicPathInfo(const int type, const QString& pathpathInfo);
//    static void setMusicPathInfo(const DeviceWatcherType type, const QString& pathInfo);
    static void setImagePathInfo(const int type, const QString& pathpathInfo);
//    static void setImagePathInfo(const DeviceWatcherType type, const QString& pathInfo);
    static void setVideoMillesmial(const int type, const int millesmial);
//    static void setVideoMillesmial(const DeviceWatcherType type, const int millesmial);
    static void setVideoPathInfo(const int type, const QString& pathInfo);
//    static void setVideoPathInfo(const DeviceWatcherType type, const QString& pathInfo);
    static void reset();
};

#endif // MULTIMEDIAPERSISTENT_H
