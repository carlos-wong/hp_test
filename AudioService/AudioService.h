#ifndef AUDIOSERVICE_H
#define AUDIOSERVICE_H

#define AudioApplication             QString("-audio")
#define ArkMicroAudioService         QString("com.arkmicro.audio")
#define ArkMicroAudioPath            QString("/com/arkmicro/audio")
#define ArkMicroAudioInterface       QString("Local.DbusServer.Audio")
#define ArkMicroAudioRequest         QString("requestAudioSource")
#define ArkMicroAudioRelease         QString("releaseAudioSource")

#include "AudioServiceProxy.h"
#include "AudioPersistent.h"
#include <QObject>
#include <QScopedPointer>

class AudioServicePrivate;
class AudioService : private QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(AudioService)
    Q_CLASSINFO("D-Bus Interface", "Local.DbusServer.Audio")
public:
    explicit AudioService(QObject* parent = NULL);
    ~AudioService();
public slots:
    void requestAudioSource(const int source);
    void releaseAudioSource(const int source);
    void reqesetReset();
    void requestMuteToggole();
    void requestSpeaker(const int item);
    void requestMute(const int item);
    void requestIncreaseHalfVolume();
    void requestDecreaseHalfVolume();
    void requestIncreaseVolume();
    void requestDecreaseVolume();
    void requestSetVolume(const int volume);
    void setEqualizerItem(const int item, const int bass, const int middle, const int treble);
    void setSoundItem(const int item, const int left, const int right);
signals:
    void onMuteChange(const int mute);
    void onVolumeChange(const int volume);
    void onEqualizerItemChange(const int item, const int bass, const int middle, const int treble);
    void onSoundItemChange(const int item, const int left, const int right);
private:
    Q_DECLARE_PRIVATE(AudioService)
    AudioServicePrivate* const d_ptr;
};

#endif // AUDIOSERVICE_H
