#ifndef AUDIOSERVICEPROXY_H
#define AUDIOSERVICEPROXY_H

#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>
#include "AudioService.h"

/*
 * Proxy class for interface Local.DbusServer.Audio
 */
class LocalDbusServerAudioInterface: public QDBusAbstractInterface
{
    Q_OBJECT
public:
    static inline const char *staticInterfaceName()
    { return "Local.DbusServer.Audio"; }

public:
    LocalDbusServerAudioInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = 0);

    ~LocalDbusServerAudioInterface();

public Q_SLOTS: // METHODS
    inline QDBusPendingReply<> releaseAudioSource(int source)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(source);
        return asyncCallWithArgumentList(QLatin1String("releaseAudioSource"), argumentList);
    }

    inline QDBusPendingReply<> reqesetReset()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QLatin1String("reqesetReset"), argumentList);
    }

    inline QDBusPendingReply<> requestAudioSource(int source)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(source);
        return asyncCallWithArgumentList(QLatin1String("requestAudioSource"), argumentList);
    }

    inline QDBusPendingReply<> requestDecreaseVolume()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QLatin1String("requestDecreaseVolume"), argumentList);
    }

    inline QDBusPendingReply<> requestIncreaseVolume()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QLatin1String("requestIncreaseVolume"), argumentList);
    }

    inline QDBusPendingReply<> requestMuteToggole()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QLatin1String("requestMuteToggole"), argumentList);
    }

    inline QDBusPendingReply<> requestSpeaker(int item)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(item);
        return asyncCallWithArgumentList(QLatin1String("requestSpeaker"), argumentList);
    }

    inline QDBusPendingReply<> requestMute(int item)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(item);
        return asyncCallWithArgumentList(QLatin1String("requestMute"), argumentList);
    }

    inline QDBusPendingReply<> requestIncreaseHalfVolume()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QLatin1String("requestIncreaseHalfVolume"), argumentList);
    }
    inline QDBusPendingReply<> requestDecreaseHalfVolume()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QLatin1String("requestMute"), argumentList);
    }

    inline QDBusPendingReply<> requestSetVolume(int volume)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(volume);
        return asyncCallWithArgumentList(QLatin1String("requestSetVolume"), argumentList);
    }

    inline QDBusPendingReply<> setEqualizerItem(int item, int bass, int middle, int treble)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(item) << qVariantFromValue(bass) << qVariantFromValue(middle) << qVariantFromValue(treble);
        return asyncCallWithArgumentList(QLatin1String("setEqualizerItem"), argumentList);
    }

    inline QDBusPendingReply<> setSoundItem(int item, int left, int right)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(item) << qVariantFromValue(left) << qVariantFromValue(right);
        return asyncCallWithArgumentList(QLatin1String("setSoundItem"), argumentList);
    }

Q_SIGNALS: // SIGNALS
    void onEqualizerItemChange(int item, int bass, int middle, int treble);
    void onMuteChange(int mute);
    void onSoundItemChange(int item, int left, int right);
    void onVolumeChange(int volume);
};

namespace Local {
  namespace DbusServer {
    typedef ::LocalDbusServerAudioInterface Audio;
  }
}
#endif
