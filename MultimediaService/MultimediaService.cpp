#include "MultimediaService.h"
#include "AutoConnect.h"
#include "DeviceWatcher/DeviceWatcher.h"
#include "MusicPlayer/MusicPlayer.h"
#include "VideoPlayer/VideoPlayer.h"
#include "ImagePlayer/ImagePlayer.h"
#include "UserInterfaceUtility.h"
#include <QCoreApplication>
#include <QDBusConnection>

class MultimediaServicePrivate
{
    Q_DISABLE_COPY(MultimediaServicePrivate)
public:
    explicit MultimediaServicePrivate(MultimediaService* parent);
    ~MultimediaServicePrivate();
    void initialize();
    void connectAllSlots();
private:
    MultimediaService* m_Parent;
};

MultimediaService::MultimediaService(QObject *parent)
    : QObject(parent)
    , m_Private(new MultimediaServicePrivate(this))
{
}

MultimediaService::~MultimediaService()
{
}

void MultimediaService::synchronize()
{
    g_DeviceWatcher->synchronize();
}

void MultimediaService::reset()
{
    MultimediaPersistent::reset();
}

void MultimediaService::musicPlayerRequestFileNames()
{
    g_MusicPlayer->musicPlayerRequestFileNames(DWT_USBDisk);
}

void MultimediaService::deviceWatcherCheckStatus(const DeviceWatcherType type)
{
    g_DeviceWatcher->deviceWatcherCheckStatus(type);
}

void MultimediaService::musicPlayerSetPlayModeToggle()
{
    g_MusicPlayer->musicPlayerSetPlayModeToggle();
}

void MultimediaService::musicPlayerSetPlayMode(const MusicPlayerPlayMode mode)
{
    g_MusicPlayer->musicPlayerSetPlayMode(mode);
}

void MultimediaService::musicPlayerSetPlayStatusToggle()
{
    g_MusicPlayer->musicPlayerSetPlayStatusToggle();
}

void MultimediaService::musicPlayerSetPlayStatus(const MusicPlayerPlayStatus status)
{
    g_MusicPlayer->musicPlayerSetPlayStatus(status);
}

void MultimediaService::musicPlayerPlayListViewIndex(const DeviceWatcherType type, const int index, const int millesimal)
{
    g_MusicPlayer->musicPlayerPlayListViewIndex(type, index, millesimal);
}

void MultimediaService::musicPlayerPlayPreviousListViewIndex()
{
    g_MusicPlayer->musicPlayerPlayPreviousListViewIndex();
}

void MultimediaService::musicPlayerPlayNextListViewIndex()
{
    g_MusicPlayer->musicPlayerPlayNextListViewIndex();
}

void MultimediaService::musicPlayerSeekToMillesimal(const int millesimal)
{
    g_MusicPlayer->musicPlayerSeekToMillesimal(millesimal);
}

void MultimediaService::musicPlayerExit()
{
    g_MusicPlayer->musicPlayerExit();
}

void MultimediaService::imagePlayerRequestFileNames(const DeviceWatcherType type)
{
    g_ImagePlayer->imagePlayerRequestFileNames(type);
}

void MultimediaService::imagePlayerPlayListViewIndex(const DeviceWatcherType type, const int index)
{
    g_ImagePlayer->imagePlayerPlayListViewIndex(type, index);
}

void MultimediaService::imagePlayerPlayPreviousListViewIndex()
{
    g_ImagePlayer->imagePlayerPlayPreviousListViewIndex();
}

void MultimediaService::imagePlayerPlayNextListViewIndex()
{
    g_ImagePlayer->imagePlayerPlayNextListViewIndex();
}

void MultimediaService::imagePlayerRotateImage()
{
    g_ImagePlayer->imagePlayerRotateImage();
}

void MultimediaService::imagePlayerZoomInImage()
{
    g_ImagePlayer->imagePlayerZoomInImage();
}

void MultimediaService::imagePlayerZoomOutImage()
{
    g_ImagePlayer->imagePlayerZoomOutImage();
}

void MultimediaService::videoPlayerRequestFileNames(const DeviceWatcherType type)
{
    g_VideoPlayer->videoPlayerRequestFileNames(type);
}

void MultimediaService::videoPlayerSetPlayModeToggle()
{
    g_VideoPlayer->videoPlayerSetPlayModeToggle();
}

void MultimediaService::videoPlayerSetPlayStatusToggle()
{
    g_VideoPlayer->videoPlayerSetPlayStatusToggle();
}

void MultimediaService::videoPlayerSetPlayStatus(const VideoPlayerPlayStatus status)
{
    g_VideoPlayer->videoPlayerSetPlayStatus(status);
}

void MultimediaService::videoPlayerPlayListViewIndex(const DeviceWatcherType type, const int index, const int x, const int y, const int width, const int height, const int millesimal)
{
    g_VideoPlayer->videoPlayerPlayListViewIndex(type, index, x, y, width, height, millesimal);
}

void MultimediaService::videoPlayerPlayPreviousListViewIndex()
{
    g_VideoPlayer->videoPlayerPlayPreviousListViewIndex();
}

void MultimediaService::videoPlayerPlayNextListViewIndex()
{
    g_VideoPlayer->videoPlayerPlayNextListViewIndex();
}

void MultimediaService::videoPlayerSeekToMillesimal(const int millesimal)
{
    g_VideoPlayer->videoPlayerSeekToMillesimal(millesimal);
}

void MultimediaService::videoPlayerExit()
{
    g_VideoPlayer->videoPlayerExit();
}

MultimediaServicePrivate::MultimediaServicePrivate(MultimediaService* parent)
    : m_Parent(parent)
{
    initialize();
    connectAllSlots();
}

MultimediaServicePrivate::~MultimediaServicePrivate()
{
}

void MultimediaServicePrivate::initialize()
{
    qDebug() << "MultimediaServicePrivate::initialize";
    bool ret = QDBusConnection::sessionBus().registerService(ArkMicroMultimediaService);
    ret = ret && QDBusConnection::sessionBus().registerObject(ArkMicroMultimediaPath,
                                                              m_Parent,
                                                              QDBusConnection::ExportNonScriptableSignals
                                                              | QDBusConnection::ExportNonScriptableSlots);
    if (!ret) {
        qCritical() << "MultimediaService Register QDbus failed!";
        exit(EXIT_FAILURE);
    }
}

void MultimediaServicePrivate::connectAllSlots()
{
    connectSignalAndSignalByNamesake(g_MusicPlayer, m_Parent, SIGNAL(onMusicPlayerPlayMode(const int)));
    connectSignalAndSignalByNamesake(g_MusicPlayer, m_Parent, SIGNAL(onMusicPlayerPlayStatus(const int, const int)));
    connectSignalAndSignalByNamesake(g_MusicPlayer, m_Parent, SIGNAL(onMusicPlayerElapsedInformation(const int, const int)));
    connectSignalAndSignalByNamesake(g_MusicPlayer, m_Parent, SIGNAL(onMusicPlayerID3TagChange(const int, const int, const QString&, const QString&, const QString&, const QString&, const int)));
    connectSignalAndSignalByNamesake(g_MusicPlayer, m_Parent, SIGNAL(onMusicPlayerFileNames(const int, const QString &)));
    connectSignalAndSignalByNamesake(g_ImagePlayer, m_Parent, SIGNAL(onImagePlayerFileNames(const int, const QString &)));
    connectSignalAndSignalByNamesake(g_ImagePlayer, m_Parent, SIGNAL(onImagePlayerChange(const int, const QString &, const int, const int, const int)));
    connectSignalAndSignalByNamesake(g_VideoPlayer, m_Parent, SIGNAL(onVideoPlayerPlayMode(const int)));
    connectSignalAndSignalByNamesake(g_VideoPlayer, m_Parent, SIGNAL(onVideoPlayerShowStatus(const int)));
    connectSignalAndSignalByNamesake(g_VideoPlayer, m_Parent, SIGNAL(onVideoPlayerPlayStatus(const int, const int)));
    connectSignalAndSignalByNamesake(g_VideoPlayer, m_Parent, SIGNAL(onVideoPlayerFileNames(const int, const QString &)));
    connectSignalAndSignalByNamesake(g_VideoPlayer, m_Parent, SIGNAL(onVideoPlayerInformation(const int, const int, const QString &, const int)));
    connectSignalAndSignalByNamesake(g_VideoPlayer, m_Parent, SIGNAL(onVideoPlayerElapsedInformation(const int, const int)));
    connectSignalAndSignalByNamesake(g_DeviceWatcher, m_Parent, SIGNAL(onDeviceWatcherStatus(const int, const int)));
}
