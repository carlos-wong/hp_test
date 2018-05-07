#ifndef MUSICPLAYER_H
#define MUSICPLAYER_H

#include "DeviceWatcher/DeviceWatcher.h"
#include <QObject>
#include <QImage>
#include <QScopedPointer>

class MusicPlayerPrivate;
class MusicPlayer : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(MusicPlayer)
#ifdef g_MusicPlayer
#undef g_MusicPlayer
#endif
#define g_MusicPlayer (MusicPlayer::instance())
public:
    inline static MusicPlayer* instance() {
        static MusicPlayer* musicPlayer(new MusicPlayer(qApp));
        return musicPlayer;
    }
    void musicPlayerRequestFileNames(const int type);
    void musicPlayerSetPlayModeToggle();
    void musicPlayerSetPlayMode(const int mode);
    void musicPlayerSetPlayStatusToggle();
    void musicPlayerSetPlayStatus(const int status);
    void musicPlayerPlayListViewIndex(const int type, const int index, const int millesimal);
    void musicPlayerPlayPreviousListViewIndex();
    void musicPlayerPlayNextListViewIndex();
    void musicPlayerSeekToMillesimal(const int millesimal);
    void musicPlayerExit();
signals:
    void onMusicPlayerPlayMode(const int mode);
    void onMusicPlayerPlayStatus(const int type, const int status);
    void onMusicPlayerElapsedInformation(const int elapsedTime,
                                         const int elapsedMillesimal);
    void onMusicPlayerID3TagChange(const int type,
                                   const int index,
                                   const QString& fileName,
                                   const QString& title,
                                   const QString& artist,
                                   const QString& album,
                                   const int endTime);
    void onMusicPlayerFileNames(const int type, const QString &xml);
protected slots:
    void onDeviceWatcherStatus(const int type, const int status);
    void onMusicFilePath(const QString &path, const int type);
private slots:
    void onTimeout();
    void onReadyReadStandardOutput();
    void onReadyReadStandardError();
    void onStarted();
    void onFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onError(QProcess::ProcessError error);
    void onStateChanged(QProcess::ProcessState state);
private:
    explicit MusicPlayer(QObject* parent = NULL);
    ~MusicPlayer();
    friend class MusicPlayerPrivate;
    QScopedPointer<MusicPlayerPrivate> m_Private;
};

#endif // MUSICPLAYER_H
