#ifndef AUDIO_H
#define AUDIO_H

#include "AudioService.h"
#include "DbusService.h"
#include <QObject>
#include <QString>
#include <QScopedPointer>
#include <QCoreApplication>

#include <QObject>
#include <QScopedPointer>

class AudioPrivate;
class Audio
        : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(Audio)
#ifdef g_Audio
#undef g_Audio
#endif
#define g_Audio (Audio::instance())
public:
    inline static Audio* instance() {
        static Audio* audio(new Audio(qApp));
        return audio;
    }
    bool requestAudioSource(const int source);
    void releaseAudioSource(const int source);
    void reset();
    void requestMuteToggole();
    void requestSpeaker(const int item);
    void requestMute(const int item);
    void requestIncreaseVolume();
    void requestDecreaseVolume();
    void requestSetVolume(const int volume);
    void setEqualizerItem(const int item, const int bass, const int middle, const int treble);
    void setSoundItem(const int item, const int left, const int right);
    AudioSource getPhoneSource();
    AudioSource getAuxSource();
    AudioSource getMultimediaSource();
    AudioSource getAudioSource();
    MuteItem getMute();
signals:
    void onMuteChange(const int mute);
    void onVolumeChange(const int volume);
    void onEqualizerItemChange(const int item, const int bass, const int middle, const int treble);
    void onSoundItemChange(const int item, const int left, const int right);
    void onHolderChange(const int oldHolder, const int newHolder);
private slots:
    void onServiceUnregistered(const QString& service);
    void onMuteChangeHandler(const int mute);
private:
    explicit Audio(QObject* parent = NULL);
    ~Audio();
    void initializePrivate();
    friend class AudioPrivate;
    QScopedPointer<AudioPrivate> m_Private;
};

#endif // AUDIO_H
