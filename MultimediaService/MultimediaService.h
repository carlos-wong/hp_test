#ifndef MULTIMEDIASERVICE_H
#define MULTIMEDIASERVICE_H

#include "MultimediaServiceProxy.h"
#include "MultimediaPersistent.h"
#include <QObject>
#include <QScopedPointer>

#define MultimediaApplication                   QString("-multimedia")
#define ArkMicroMultimediaService               QString("com.arkmicro.multimedia")
#define ArkMicroMultimediaPath                  QString("/com/arkmicro/multimedia")
#define ArkMicroMultimediaInterface             QString("Local.DbusServer.Multimedia")

class MultimediaServicePrivate;
class MultimediaService : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(MultimediaService)
    Q_CLASSINFO("D-Bus Interface", "Local.DbusServer.Multimedia")
#ifdef g_MultimediaService
#undef g_MultimediaService
#endif
#define g_MultimediaService (MultimediaService::instance())
public:
    explicit MultimediaService(QObject* parent = NULL);
    ~MultimediaService();
#ifdef gcc
public slots:
   // void usbToggle(const int action);
   // void sdToggle(const int action);
#endif
public slots:
    void synchronize();
    void reset();
    void musicPlayerRequestFileNames();
    void deviceWatcherCheckStatus(const int type);
    void musicPlayerSetPlayModeToggle();
    void musicPlayerSetPlayMode(const int mode);
    void musicPlayerSetPlayStatusToggle();
    void musicPlayerSetPlayStatus(const int status);
    void musicPlayerPlayListViewIndex(const int type, const int index, const int millesimal);
    void musicPlayerPlayPreviousListViewIndex();
    void musicPlayerPlayNextListViewIndex();
    void musicPlayerSeekToMillesimal(const int millesimal);
    void musicPlayerExit();
    void imagePlayerRequestFileNames(const int type);
    void imagePlayerPlayListViewIndex(const int type, const int index);
    void imagePlayerPlayPreviousListViewIndex();
    void imagePlayerPlayNextListViewIndex();
    void imagePlayerRotateImage();
    void imagePlayerZoomInImage();
    void imagePlayerZoomOutImage();
    void videoPlayerRequestFileNames(const int type);
    void videoPlayerSetPlayModeToggle();
    void videoPlayerSetPlayStatusToggle();
    void videoPlayerSetPlayStatus(const int status);
    void videoPlayerPlayListViewIndex(const int type, const int index, const int x, const int y, const int width, const int height, const int millesimal);
    void videoPlayerPlayPreviousListViewIndex();
    void videoPlayerPlayNextListViewIndex();
    void videoPlayerSeekToMillesimal(const int millesimal);
    void videoPlayerExit();
signals:
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
    void onImagePlayerChange(const int type, const QString &filePath, const int index, const int percent, const int rotate);
    void onVideoPlayerPlayMode(const int mode);
    void onVideoPlayerShowStatus(const int status);
    void onVideoPlayerPlayStatus(const int type, const int status);
    void onVideoPlayerFileNames(const int type, const QString& xml);
    void onVideoPlayerInformation(const int type, const int index, const QString &fileName, const int endTime);
    void onVideoPlayerElapsedInformation(const int elapsedTime, const int elapsedMillesimal);
private:
    friend class MultimediaServicePrivate;
    QScopedPointer<MultimediaServicePrivate> m_Private;
};

#endif // MULTIMEDIASERVICE_H
