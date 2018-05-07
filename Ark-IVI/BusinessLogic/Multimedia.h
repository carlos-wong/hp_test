#ifndef MULTIMEDIA_H
#define MULTIMEDIA_H

#include "MultimediaService.h"
#include "BusinessLogic/Audio.h"
#include "DbusService.h"
#include <QObject>
#include <QScopedPointer>

class MultimediaPrivate;
class Multimedia
        : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(Multimedia)
#ifdef g_Multimedia
#undef g_Multimedia
#endif
#define g_Multimedia (Multimedia::instance())
public:
    inline static Multimedia* instance() {
        static Multimedia* multimedia(new Multimedia(qApp));
        return multimedia;
    }
    enum Status {
        S_Undefine = -1,
        S_Start,
        S_Stop,
    };
    enum ImageFocus {
        F_Undefine = -1,
        F_ImageSDFocus,
        F_ImageUSBFocus,
        F_ImageUnfocus,
    };
    void startMultimedia();
    void musicInitialize();
    void imageInitliaze();
    void videoInitialize();
    void setImageFocus(const Multimedia::ImageFocus focus);
    void reset();
    void musicPlayerSetPlayModeToggle();
    void musicPlayerSetPlayMode(const int mode);
    void musicPlayerSetPlayStatusToggle();
    void musicPlayerSetPlayStatus(const int status);
    void musicPlayerPlayListViewIndex(const int type, const int index, const int millesimal);
    void musicPlayerPlayPreviousListViewIndex();
    void musicPlayerPlayNextListViewIndex();
    void musicPlayerSeekToMillesimal(const int millesimal);
    void musicPlayerExit();
    void imagePlayerPlayListViewIndex(const int type, const int index);
    void imagePlayerPlayPreviousListViewIndex();
    void imagePlayerPlayNextListViewIndex();
    void imagePlayerRotateImage();
    void imagePlayerZoomInImage();
    void imagePlayerZoomOutImage();
    void videoPlayerSetPlayModeToggle();
    void videoPlayerSetPlayStatusToggle();
    void videoPlayerSetPlayStatus(const int status);
    void videoPlayerPlayListViewIndex(const int type, const int index, const int x, const int y, const int width, const int height, const int millesimal) ;
    void videoPlayerPlayPreviousListViewIndex();
    void videoPlayerPlayNextListViewIndex() ;
    void videoPlayerSeekToMillesimal(const int millesimal);
    void videoPlayerExit();
    Multimedia::ImageFocus getImageFocus();
    DeviceWatcherStatus getDeviceStatus(const DeviceWatcherType type);
    DeviceWatcherType getDeviceActiveType();
    bool musicListEmpty(const DeviceWatcherType type);
    bool imageListEmpty(const DeviceWatcherType type);
    bool videoListEmpty(const DeviceWatcherType type);
signals:
    void onMultimediaChange(const Multimedia::Status status);
    void onDeviceWatcherStatus(const int type, const int status);
    void onMusicPlayerPlayMode(const int mode);
    void onMusicPlayerPlayStatus(const int type, const int status);
    void onMusicPlayerElapsedInformation(const int elapsedTime,
                                         const int elapsedMillesimal);
    void onMusicPlayerID3TagChange(const int type,
                                   const int index,
                                   const QString &fileName,
                                   const QString& title,
                                   const QString& artist,
                                   const QString& album,
                                   const int endTime);
    void onMusicPlayerFileNames(const int type, const QString &xml);
    void onImagePlayerFileNames(const int type, const QString &xml);
    void onImagePlayerPlayStatus(const int type, const int status);
    void onImagePlayerChange(const int type, const QString &filePath, const int index, const int percent, const int rotate);
    void onVideoPlayerPlayMode(const int mode);
    void onVideoPlayerShowStatus(const int status);
    void onVideoPlayerPlayStatus(const int type, const int status);
    void onVideoPlayerFileNames(const int type, const QString& xml);
    void onVideoPlayerInformation(const int type, const int index, const QString &fileName, const int endTime);
    void onVideoPlayerElapsedInformation(const int elapsedTime, const int elapsedMillesimal);
private slots:
    void onDeviceWatcherStatusHandler(const int type, const int status);
    void onMusicPlayerFileNamesHandler(const int type, const QString &xml);
    void onImagePlayerFileNamesHandler(const int type, const QString &xml);
    void onVideoPlayerFileNamesHandler(const int type, const QString& xml);
    void onMusicPlayerPlayStatusHandler(const int type, const int status);
    void onImagePlayerChangeHandler(const int type, const QString &filePath, const int index, const int percent, const int rotate);
    void onVideoPlayerPlayStatusHandler(const int type, const int status);
    void onHolderChange(const int oldHolder, const int newHolder);
private:
    explicit Multimedia(QObject* parent = NULL);
    ~Multimedia();
    void initializePrivate();
    friend class MultimediaPrivate;
    QScopedPointer<MultimediaPrivate> m_Private;
};

#endif // MULTIMEDIA_H
