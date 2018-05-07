#include "BusinessLogic/Audio.h"
#include "AutoConnect.h"
#include "BusinessLogic/BusinessLogicUtility.h"
#include "AudioServiceProxy.h"
#include "Utility.h"
#include "UserInterfaceUtility.h"
#include "ArkApplication.h"
#include <QDBusConnection>
#include <QProcess>

class AudioPrivate
{
public:
    explicit AudioPrivate(Audio* parent);
    ~AudioPrivate();
    void initialize();
    void connectAllSlots();
    AudioSource m_Multimedia;
    AudioSource m_Aux;
    AudioSource m_Phone;
    Local::DbusServer::Audio* m_AudioServiceProxy;
    MuteItem m_MuteItem;
private:
    Audio* m_Parent;
};

bool Audio::requestAudioSource(const AudioSource source)
{
    qDebug() << "Audio::requestAudioSource" << source << m_Private->m_Phone << m_Private->m_Multimedia;
    initializePrivate();
    static bool flag(false);
    bool ret = true;
    if (!flag) {
        flag = true;
        QStringList cmd;
        cmd << QString("-t") << AudioApplication;
        bool result = QProcess::startDetached(ArkApp->applicationFilePath(), cmd);
        qDebug() << __PRETTY_FUNCTION__ << __LINE__ << "result" << result;
    } else {
        if ((AS_BluetoothPhone != source)
                && (AS_CarplayPhone != source)
                && (AS_CarlifePhone != source)
                && (AS_AutoPhone != source)) {
            if (AS_Idle == m_Private->m_Phone) {
                qDebug() << __PRETTY_FUNCTION__ << __LINE__;
                if (AS_Idle == m_Private->m_Aux) {
                    if (source != m_Private->m_Multimedia) {
                        qDebug() << __PRETTY_FUNCTION__ << __LINE__;
                        AudioSource multimedia = m_Private->m_Multimedia;
                        if (AS_Aux == source) {
                            m_Private->m_Aux = AS_Aux;
                        } else {
                            m_Private->m_Multimedia = source;
                        }
                        onHolderChange(multimedia, source);
                    }
                    QDBusPendingReply<> reply = m_Private->m_AudioServiceProxy->requestAudioSource(source);
                    reply.waitForFinished();
                } else {
                    if (AS_Aux != source) {
                        m_Private->m_Multimedia = source;
                        ret = false;
                    }
                }
            } else {
                qDebug() << __PRETTY_FUNCTION__ << __LINE__;
                if (AS_Aux == source) {
                    m_Private->m_Aux = AS_Aux;
                } else {
                    m_Private->m_Multimedia = source;
                }
                ret = false;
            }
        } else {
            qDebug() << __PRETTY_FUNCTION__ << __LINE__;
            if ((AS_BluetoothPhone == source)
                    && (AS_BluetoothPhone != m_Private->m_Phone)) {
                qDebug() << __PRETTY_FUNCTION__ << __LINE__;
                m_Private->m_Phone = AS_BluetoothPhone;
                onHolderChange(m_Private->m_Multimedia, source);
                QDBusPendingReply<> reply = m_Private->m_AudioServiceProxy->requestAudioSource(source);
                reply.waitForFinished();
            } else if ((AS_CarplayPhone == source)
                       && (AS_CarplayPhone != m_Private->m_Phone)) {
                m_Private->m_Phone = AS_CarplayPhone;
                onHolderChange(m_Private->m_Multimedia, source);
                QDBusPendingReply<> reply = m_Private->m_AudioServiceProxy->requestAudioSource(source);
                reply.waitForFinished();
            } else if ((AS_CarlifePhone == source)
                       && (AS_CarlifePhone != m_Private->m_Phone)) {
                qDebug() << __PRETTY_FUNCTION__ << __LINE__;
                m_Private->m_Phone = AS_CarlifePhone;
                onHolderChange(m_Private->m_Multimedia, source);
                QDBusPendingReply<> reply = m_Private->m_AudioServiceProxy->requestAudioSource(source);
                reply.waitForFinished();
            } else if ((AS_AutoPhone == source)
                       && (AS_AutoPhone != m_Private->m_Phone)) {
                qDebug() << __PRETTY_FUNCTION__ << __LINE__;
                m_Private->m_Phone = AS_AutoPhone;
                onHolderChange(m_Private->m_Multimedia, source);
                QDBusPendingReply<> reply = m_Private->m_AudioServiceProxy->requestAudioSource(source);
                reply.waitForFinished();
            }
        }
    }
    return ret;
}

void Audio::releaseAudioSource(const AudioSource source)
{
    qDebug() << "Audio::releaseAudioSource" << source << m_Private->m_Multimedia;
    if ((AS_BluetoothPhone == source)
            || (AS_CarplayPhone == source)
            || (AS_CarlifePhone == source)
            || (AS_AutoPhone == source)) {
        if ((AS_BluetoothPhone == m_Private->m_Phone)
                || (AS_CarplayPhone == m_Private->m_Phone)
                || (AS_CarlifePhone == m_Private->m_Phone)
                || (AS_AutoPhone == m_Private->m_Phone)) {
            if (AS_Aux == m_Private->m_Aux) {
                onHolderChange(source, AS_Aux);
            } else {
                m_Private->m_Phone = AS_Idle;
                if (AS_Idle != m_Private->m_Multimedia) {
                    qDebug() << __PRETTY_FUNCTION__ << __LINE__ << m_Private->m_Multimedia;
                    onHolderChange(source, m_Private->m_Multimedia);
                }
            }
        }
    } else if (AS_Aux == source) {
        if (AS_Aux == m_Private->m_Aux) {
            m_Private->m_Aux = AS_Idle;
            if (AS_Idle != m_Private->m_Multimedia) {
                qDebug() << __PRETTY_FUNCTION__ << __LINE__ << m_Private->m_Multimedia;
                onHolderChange(source, m_Private->m_Multimedia);
            }
        }
    }
}

void Audio::reset()
{
    initializePrivate();
    QDBusPendingReply<> reply = m_Private->m_AudioServiceProxy->reqesetReset();
    reply.waitForFinished();
    if (reply.isError()) {
        qDebug() << "method call customEvent failed" << reply.error();
    }
}

void Audio::requestMuteToggole()
{
    initializePrivate();
    QDBusPendingReply<> reply = m_Private->m_AudioServiceProxy->requestMuteToggole();
    reply.waitForFinished();
    if (reply.isError()) {
        qDebug() << __PRETTY_FUNCTION__ << reply.error();
    }
}

void Audio::requestSpeaker(const int item)
{
    initializePrivate();
    QDBusPendingReply<> reply = m_Private->m_AudioServiceProxy->requestSpeaker(item);
    reply.waitForFinished();
    if (reply.isError()) {
        qDebug() << __PRETTY_FUNCTION__ << reply.error();
    }
}

void Audio::requestMute(const int item)
{
    initializePrivate();
    QDBusPendingReply<> reply = m_Private->m_AudioServiceProxy->requestMute(item);
    reply.waitForFinished();
    if (reply.isError()) {
        qDebug() << __PRETTY_FUNCTION__ << reply.error();
    }
}


void Audio::requestIncreaseVolume()
{
    initializePrivate();
    QDBusPendingReply<> reply = m_Private->m_AudioServiceProxy->requestIncreaseVolume();
    reply.waitForFinished();
    if (reply.isError()) {
        qDebug() << "method call customEvent failed" << reply.error();
    }
}

void Audio::requestDecreaseVolume()
{
    initializePrivate();
    QDBusPendingReply<> reply = m_Private->m_AudioServiceProxy->requestDecreaseVolume();
    reply.waitForFinished();
    if (reply.isError()) {
        qDebug() << "method call customEvent failed" << reply.error();
    }
}

void Audio::requestSetVolume(const int volume)
{
    initializePrivate();
    QDBusPendingReply<> reply = m_Private->m_AudioServiceProxy->requestSetVolume(volume);
    reply.waitForFinished();
    if (reply.isError()) {
        qDebug() << "method call customEvent failed" << reply.error();
    }
}

void Audio::setEqualizerItem(const int item, const int bass, const int middle, const int treble)
{
    initializePrivate();
    QDBusPendingReply<> reply = m_Private->m_AudioServiceProxy->setEqualizerItem(item, bass, middle, treble);
    reply.waitForFinished();
    if (reply.isError()) {
        qDebug() << "method call customEvent failed" << reply.error();
    }
}

void Audio::setSoundItem(const int item, const int left, const int right)
{
    initializePrivate();
    QDBusPendingReply<> reply = m_Private->m_AudioServiceProxy->setSoundItem(item, left, right);
    reply.waitForFinished();
    if (reply.isError()) {
        qDebug() << "method call customEvent failed" << reply.error();
    }
}

AudioSource Audio::getPhoneSource()
{
    return m_Private->m_Phone;
}

AudioSource Audio::getAuxSource()
{
    return m_Private->m_Aux;
}

AudioSource Audio::getMultimediaSource()
{
    return m_Private->m_Multimedia;
}

AudioSource Audio::getAudioSource()
{
    bool ret1 = (AS_BluetoothPhone == m_Private->m_Phone)
            || (AS_CarplayPhone == m_Private->m_Phone)
            || (AS_CarlifePhone == m_Private->m_Phone)
            || (AS_AutoPhone == m_Private->m_Phone);
    //    qDebug() << __PRETTY_FUNCTION__ << (ret1? m_Private->m_Phone: m_Private->m_Multimedia);
    bool ret2 = (!ret1) && (AS_Aux == m_Private->m_Aux);
    return (ret1? m_Private->m_Phone: (ret2? m_Private->m_Aux: m_Private->m_Multimedia));
}

MuteItem Audio::getMute()
{
    return m_Private->m_MuteItem;
}

void Audio::onServiceUnregistered(const QString &service)
{
    qDebug() << "Audio::onServiceUnregistered" << service;
    if (ArkMicroAudioService == service) {
        g_DbusService->startService(ArkMicroAudioService);
    }
}

void Audio::onMuteChangeHandler(const MuteItem mute)
{
    m_Private->m_MuteItem = mute;
    onMuteChange(m_Private->m_MuteItem);
}

Audio::Audio(QObject *parent)
    : QObject(parent)
    , m_Private(new AudioPrivate(this))
{
}

Audio::~Audio()
{
}

void Audio::initializePrivate()
{
    if (NULL == m_Private) {
        m_Private.reset(new AudioPrivate(this));
    }
}

static void test(void * ptr)
{

}

AudioPrivate::AudioPrivate(Audio *parent)
    : m_Parent(parent)
{
    m_Multimedia = AS_Radio;
    m_Aux = AS_Idle;
    m_Phone = AS_Idle;
    m_AudioServiceProxy = NULL;
    m_MuteItem = MI_Unmute;
    initialize();
    connectAllSlots();
}

AudioPrivate::~AudioPrivate()
{
}

void AudioPrivate::initialize()
{
    if (NULL == m_AudioServiceProxy) {
        m_AudioServiceProxy = new Local::DbusServer::Audio(ArkMicroAudioService,
                                                           ArkMicroAudioPath,
                                                           QDBusConnection::sessionBus(),
                                                           m_Parent);
    }
}

void AudioPrivate::connectAllSlots()
{
    if (NULL != m_AudioServiceProxy) {
        connectSignalAndSignalByNamesake(m_AudioServiceProxy, m_Parent, ARKSENDER(onVolumeChange(const int)));
        connectSignalAndSignalByNamesake(m_AudioServiceProxy, m_Parent, ARKSENDER(onEqualizerItemChange(const int, const int, const int, const int)));
        connectSignalAndSignalByNamesake(m_AudioServiceProxy, m_Parent, ARKSENDER(onSoundItemChange(const int, const int, const int)));
        QObject::connect(m_AudioServiceProxy, ARKSENDER(onMuteChange(const int)),
                         m_Parent,            ARKRECEIVER(onMuteChangeHandler(const int)));
    }
}

