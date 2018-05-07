#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include "DeviceWatcher/DeviceWatcher.h"
#include <QObject>
#include <QProcess>
#include <QScopedPointer>

class VideoPlayerPrivate;
class VideoPlayer : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(VideoPlayer)
#ifdef g_VideoPlayer
#undef g_VideoPlayer
#endif
#define g_VideoPlayer (VideoPlayer::instance())
public:
    inline static VideoPlayer* instance() {
        static VideoPlayer* videoPlayer(new VideoPlayer(qApp));
        return videoPlayer;
    }
    void videoPlayerRequestFileNames(const int type);
    void videoPlayerSetPlayModeToggle();
    void videoPlayerSetPlayMode(const int mode);
    void videoPlayerSetPlayStatusToggle();
    void videoPlayerSetPlayStatus(const int status);
    void videoPlayerPlayListViewIndex(const int type, const int index, const int x, const int y, const int width, const int height, const int millesimal);
    void videoPlayerPlayPreviousListViewIndex();
    void videoPlayerPlayNextListViewIndex();
    void videoPlayerSeekToMillesimal(const int millesimal);
    void videoPlayerExit();
signals:
    void onVideoPlayerPlayMode(const int mode);
    void onVideoPlayerShowStatus(const int status);
    void onVideoPlayerPlayStatus(const int type, const int status);
    void onVideoPlayerFileNames(const int type, const QString &xml);
    void onVideoPlayerInformation(const int type, const int index, const QString &fileName, const int endTime);
    void onVideoPlayerElapsedInformation(const int elapsedTime, const int elapsedMillesimal);
protected slots:
    void onDeviceWatcherStatus(const int type, const int status);
    void onVideoFilePath(const QString &path, const int type);
private slots:
    void onTimeout();
    void onReadyReadStandardOutput();
    void onReadyReadStandardError();
    void onStarted();
    void onFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onError(QProcess::ProcessError error);
    void onStateChanged(QProcess::ProcessState state);
private:
    explicit VideoPlayer(QObject* parent = NULL);
    ~VideoPlayer();
    friend class VideoPlayerPrivate;
    QScopedPointer<VideoPlayerPrivate> m_Private;
};

#endif // VIDEOPLAYER_H
