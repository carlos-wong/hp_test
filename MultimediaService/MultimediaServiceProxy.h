#ifndef MULTIMEDIASERVICEPROXY_H_1510296074
#define MULTIMEDIASERVICEPROXY_H_1510296074

#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>
#include "MultimediaService.h"

/*
 * Proxy class for interface Local.DbusServer.Multimedia
 */
class LocalDbusServerMultimediaInterface: public QDBusAbstractInterface
{
    Q_OBJECT
public:
    static inline const char *staticInterfaceName()
    { return "Local.DbusServer.Multimedia"; }

public:
    LocalDbusServerMultimediaInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = 0);

    ~LocalDbusServerMultimediaInterface();

public Q_SLOTS: // METHODS
    inline QDBusPendingReply<> deviceWatcherCheckStatus(int type)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(type);
        return asyncCallWithArgumentList(QLatin1String("deviceWatcherCheckStatus"), argumentList);
    }

    inline QDBusPendingReply<> imagePlayerPlayListViewIndex(int type, int index)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(type) << qVariantFromValue(index);
        return asyncCallWithArgumentList(QLatin1String("imagePlayerPlayListViewIndex"), argumentList);
    }

    inline QDBusPendingReply<> imagePlayerPlayNextListViewIndex()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QLatin1String("imagePlayerPlayNextListViewIndex"), argumentList);
    }

    inline QDBusPendingReply<> imagePlayerPlayPreviousListViewIndex()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QLatin1String("imagePlayerPlayPreviousListViewIndex"), argumentList);
    }

    inline QDBusPendingReply<> imagePlayerRequestFileNames(int type)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(type);
        return asyncCallWithArgumentList(QLatin1String("imagePlayerRequestFileNames"), argumentList);
    }

    inline QDBusPendingReply<> imagePlayerRotateImage()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QLatin1String("imagePlayerRotateImage"), argumentList);
    }

    inline QDBusPendingReply<> imagePlayerZoomInImage()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QLatin1String("imagePlayerZoomInImage"), argumentList);
    }

    inline QDBusPendingReply<> imagePlayerZoomOutImage()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QLatin1String("imagePlayerZoomOutImage"), argumentList);
    }

    inline QDBusPendingReply<> musicPlayerExit()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QLatin1String("musicPlayerExit"), argumentList);
    }

    inline QDBusPendingReply<> musicPlayerPlayListViewIndex(int type, int index, int millesimal)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(type) << qVariantFromValue(index) << qVariantFromValue(millesimal);
        return asyncCallWithArgumentList(QLatin1String("musicPlayerPlayListViewIndex"), argumentList);
    }

    inline QDBusPendingReply<> musicPlayerPlayNextListViewIndex()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QLatin1String("musicPlayerPlayNextListViewIndex"), argumentList);
    }

    inline QDBusPendingReply<> musicPlayerPlayPreviousListViewIndex()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QLatin1String("musicPlayerPlayPreviousListViewIndex"), argumentList);
    }

    inline QDBusPendingReply<> musicPlayerRequestFileNames()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QLatin1String("musicPlayerRequestFileNames"), argumentList);
    }

    inline QDBusPendingReply<> musicPlayerSeekToMillesimal(int millesimal)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(millesimal);
        return asyncCallWithArgumentList(QLatin1String("musicPlayerSeekToMillesimal"), argumentList);
    }

    inline QDBusPendingReply<> musicPlayerSetPlayMode(int mode)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(mode);
        return asyncCallWithArgumentList(QLatin1String("musicPlayerSetPlayMode"), argumentList);
    }

    inline QDBusPendingReply<> musicPlayerSetPlayModeToggle()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QLatin1String("musicPlayerSetPlayModeToggle"), argumentList);
    }

    inline QDBusPendingReply<> musicPlayerSetPlayStatus(int status)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(status);
        return asyncCallWithArgumentList(QLatin1String("musicPlayerSetPlayStatus"), argumentList);
    }

    inline QDBusPendingReply<> musicPlayerSetPlayStatusToggle()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QLatin1String("musicPlayerSetPlayStatusToggle"), argumentList);
    }

    inline QDBusPendingReply<> reset()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QLatin1String("reset"), argumentList);
    }

    inline QDBusPendingReply<> synchronize()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QLatin1String("synchronize"), argumentList);
    }

    inline QDBusPendingReply<> videoPlayerExit()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QLatin1String("videoPlayerExit"), argumentList);
    }

    inline QDBusPendingReply<> videoPlayerPlayListViewIndex(int type, int index, int x, int y, int width, int height, int millesimal)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(type) << qVariantFromValue(index) << qVariantFromValue(x) << qVariantFromValue(y) << qVariantFromValue(width) << qVariantFromValue(height) << qVariantFromValue(millesimal);
        return asyncCallWithArgumentList(QLatin1String("videoPlayerPlayListViewIndex"), argumentList);
    }

    inline QDBusPendingReply<> videoPlayerPlayNextListViewIndex()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QLatin1String("videoPlayerPlayNextListViewIndex"), argumentList);
    }

    inline QDBusPendingReply<> videoPlayerPlayPreviousListViewIndex()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QLatin1String("videoPlayerPlayPreviousListViewIndex"), argumentList);
    }

    inline QDBusPendingReply<> videoPlayerRequestFileNames(int type)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(type);
        return asyncCallWithArgumentList(QLatin1String("videoPlayerRequestFileNames"), argumentList);
    }

    inline QDBusPendingReply<> videoPlayerSeekToMillesimal(int millesimal)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(millesimal);
        return asyncCallWithArgumentList(QLatin1String("videoPlayerSeekToMillesimal"), argumentList);
    }

    inline QDBusPendingReply<> videoPlayerSetPlayModeToggle()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QLatin1String("videoPlayerSetPlayModeToggle"), argumentList);
    }

    inline QDBusPendingReply<> videoPlayerSetPlayStatus(int status)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(status);
        return asyncCallWithArgumentList(QLatin1String("videoPlayerSetPlayStatus"), argumentList);
    }

    inline QDBusPendingReply<> videoPlayerSetPlayStatusToggle()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QLatin1String("videoPlayerSetPlayStatusToggle"), argumentList);
    }

Q_SIGNALS: // SIGNALS
    void onDeviceWatcherStatus(int type, int status);
    void onImagePlayerChange(int type, const QString &filePath, int index, int percent, int rotate);
    void onImagePlayerFileNames(int type, const QString &xml);
    void onMusicPlayerElapsedInformation(int elapsedTime, int elapsedMillesimal);
    void onMusicPlayerFileNames(int type, const QString &xml);
    void onMusicPlayerID3TagChange(int type, int index, const QString &fileName, const QString &title, const QString &artist, const QString &album, int endTime);
    void onMusicPlayerPlayMode(int mode);
    void onMusicPlayerPlayStatus(int type, int status);
    void onVideoPlayerElapsedInformation(int elapsedTime, int elapsedMillesimal);
    void onVideoPlayerFileNames(int type, const QString &xml);
    void onVideoPlayerInformation(int type, int index, const QString &fileName, int endTime);
    void onVideoPlayerPlayMode(int mode);
    void onVideoPlayerPlayStatus(int type, int status);
    void onVideoPlayerShowStatus(int status);
};

namespace Local {
  namespace DbusServer {
    typedef ::LocalDbusServerMultimediaInterface Multimedia;
  }
}
#endif
