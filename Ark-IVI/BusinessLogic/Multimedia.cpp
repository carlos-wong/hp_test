#include "Multimedia.h"
#include "BusinessLogicUtility.h"
#include "ArkApplication.h"
#include "MultimediaService.h"
#include "AutoConnect.h"
#include "UserInterfaceUtility.h"
#include "Utility.h"
#include "BusinessLogic/Audio.h"
#include "UserInterfaceUtility.h"
#include "BusinessLogic/Setting.h"
#include "BusinessLogic/Radio.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <QProcess>
#include <QFile>
#include <QDomDocument>
#include <unistd.h>

class MultimediaPrivate
{
    Q_DISABLE_COPY(MultimediaPrivate)
public:
    explicit MultimediaPrivate(Multimedia* parent);
    ~MultimediaPrivate();
    void initializeProxy();
    bool requestAudioSource(const AudioSource source);
    void releaseAudioSource(const AudioSource source);
    void connectAllSlots();
    Local::DbusServer::Multimedia* m_MultimediaServiceProxy;
    Multimedia::ImageFocus m_ImageFocus;
    //    AudioSource m_AudioSource;
    DeviceWatcherType m_DeviceWatcherType;
    DeviceWatcherStatus m_SDStatus;
    DeviceWatcherStatus m_USBStatus;
    bool m_SDDiskMusicListEmpty;
    bool m_USBDiskMusicListEmpty;
    bool m_SDDiskImageListEmpty;
    bool m_USBDiskImageListEmpty;
    bool m_SDDiskVideoListEmpty;
    bool m_USBDiskVideoListEmpty;
private:
    Multimedia* m_Parent;
};

void Multimedia::startMultimedia()
{
    static bool flag = true;
    if (flag) {
        memallocload();
        driverload();
        flag = false;
        QStringList cmd;
        cmd << QString("-t") << MultimediaApplication;
        QProcess::startDetached(ArkApp->applicationFilePath(), cmd);
    }
}

void Multimedia::musicInitialize()
{
    struct ArkControl control;
    memset((void*)&control, 0, sizeof(struct ArkControl));
    struct ArkProtocol protocol;
    memset((void*)&protocol, 0, sizeof(struct ArkProtocol));
    protocol.type = AT_Control;
    protocol.direction = AD_Setter;
    protocol.data = &control;
    control.type = ACLT_RequestSourceSwitch;
    unsigned char data[1] = {0x03};
    control.data = data;
    control.length = 1;
    g_Setting->setArkProtocol(ArkProtocolWrapper(protocol));
    g_Audio->requestAudioSource(AS_Music);
}

void Multimedia::imageInitliaze()
{
    struct ArkControl control;
    memset((void*)&control, 0, sizeof(struct ArkControl));
    struct ArkProtocol protocol;
    memset((void*)&protocol, 0, sizeof(struct ArkProtocol));
    protocol.type = AT_Control;
    protocol.direction = AD_Setter;
    protocol.data = &control;
    control.type = ACLT_RequestSourceSwitch;
    unsigned char data[1] = {0x14};
    control.data = data;
    control.length = 1;
    g_Setting->setArkProtocol(ArkProtocolWrapper(protocol));
}

void Multimedia::videoInitialize()
{
    qDebug() << __PRETTY_FUNCTION__;
    struct ArkControl control;
    memset((void*)&control, 0, sizeof(struct ArkControl));
    struct ArkProtocol protocol;
    memset((void*)&protocol, 0, sizeof(struct ArkProtocol));
    protocol.type = AT_Control;
    protocol.direction = AD_Setter;
    protocol.data = &control;
    control.type = ACLT_RequestSourceSwitch;
    unsigned char data[1] = {0x03};
    control.data = data;
    control.length = 1;
    g_Setting->setArkProtocol(ArkProtocolWrapper(protocol));
    g_Audio->requestAudioSource(AS_Video);
}

void Multimedia::setImageFocus(const Multimedia::ImageFocus focus)
{
    m_Private->m_ImageFocus = focus;
    if (Multimedia::F_ImageSDFocus == m_Private->m_ImageFocus) {
        m_Private->m_DeviceWatcherType = DWT_SDDisk;
    } else if (Multimedia::F_ImageUSBFocus == m_Private->m_ImageFocus) {
        m_Private->m_DeviceWatcherType = DWT_USBDisk;
    }
}

void Multimedia::reset()
{
    initializePrivate();
    if (NULL != m_Private->m_MultimediaServiceProxy) {
        QDBusPendingReply<> reply = m_Private->m_MultimediaServiceProxy->reset();
        reply.waitForFinished();
        if (reply.isError()) {
            qDebug() << "method call Multimedia::reset failed" << reply.error();
        }
    }
}

void Multimedia::musicPlayerSetPlayModeToggle()
{
    initializePrivate();
    if (NULL != m_Private->m_MultimediaServiceProxy) {
        QDBusPendingReply<> reply = m_Private->m_MultimediaServiceProxy->musicPlayerSetPlayModeToggle();
        reply.waitForFinished();
        if (reply.isError()) {
            qDebug() << "method call Multimedia::musicPlayerSetPlayModeToggle failed" << reply.error();
        }
    }
}

void Multimedia::musicPlayerSetPlayMode(const int mode)
{
    initializePrivate();
    if (NULL != m_Private->m_MultimediaServiceProxy) {
        QDBusPendingReply<> reply = m_Private->m_MultimediaServiceProxy->musicPlayerSetPlayMode(mode);
        reply.waitForFinished();
        if (reply.isError()) {
            qDebug() << "method call Multimedia::musicPlayerSetPlayMode failed" << reply.error();
        }
    }
}

void Multimedia::musicPlayerSetPlayStatusToggle()
{
    initializePrivate();
    if (NULL != m_Private->m_MultimediaServiceProxy) {
        QDBusPendingReply<> reply = m_Private->m_MultimediaServiceProxy->musicPlayerSetPlayStatusToggle();
        reply.waitForFinished();
        if (reply.isError()) {
            qDebug() << "method call Multimedia::musicPlayerSetPlayStatusToggle failed" << reply.error();
        }
    }
}

void Multimedia::musicPlayerSetPlayStatus(const int status)
{
    initializePrivate();
    if (NULL != m_Private->m_MultimediaServiceProxy) {
        QDBusPendingReply<> reply = m_Private->m_MultimediaServiceProxy->musicPlayerSetPlayStatus(status);
        reply.waitForFinished();
        if (reply.isError()) {
            qDebug() << "method call Multimedia::musicPlayerSetPlayStatus failed" << reply.error();
        }
    }
}

void Multimedia::musicPlayerPlayListViewIndex(const int type, const int index, const int millesimal)
{
    initializePrivate();
    m_Private->requestAudioSource(AS_Music);
    if (NULL != m_Private->m_MultimediaServiceProxy) {
        QDBusPendingReply<void> reply = m_Private->m_MultimediaServiceProxy->musicPlayerPlayListViewIndex(type, index, millesimal);
        reply.waitForFinished();
        if (reply.isError()) {
            qDebug() << "method call Multimedia::musicPlayerPlayListViewIndex failed" << reply.error();
        }
    }
}

void Multimedia::musicPlayerPlayPreviousListViewIndex()
{
    initializePrivate();
    if (NULL != m_Private->m_MultimediaServiceProxy) {
        QDBusPendingReply<> reply = m_Private->m_MultimediaServiceProxy->musicPlayerPlayPreviousListViewIndex();
        reply.waitForFinished();
        if (reply.isError()) {
            qDebug() << "method call Multimedia::musicPlayerPlayPreviousListViewIndex failed" << reply.error();
        }
    }
}

void Multimedia::musicPlayerPlayNextListViewIndex()
{
    initializePrivate();
    if (NULL != m_Private->m_MultimediaServiceProxy) {
        QDBusPendingReply<> reply = m_Private->m_MultimediaServiceProxy->musicPlayerPlayNextListViewIndex();
        reply.waitForFinished();
        if (reply.isError()) {
            qDebug() << "method call Multimedia::musicPlayerPlayNextListViewIndex failed" << reply.error();
        }
    }
}

void Multimedia::musicPlayerSeekToMillesimal(const int millesimal)
{
    initializePrivate();
    if (NULL != m_Private->m_MultimediaServiceProxy) {
        QDBusPendingReply<> reply = m_Private->m_MultimediaServiceProxy->musicPlayerSeekToMillesimal(millesimal);
        reply.waitForFinished();
        if (reply.isError()) {
            qDebug() << "method call Multimedia::musicPlayerSeekToMillesimal failed" << reply.error();
        }
    }
}

void Multimedia::musicPlayerExit()
{
    initializePrivate();
    if (NULL != m_Private->m_MultimediaServiceProxy) {
        QDBusPendingReply<> reply = m_Private->m_MultimediaServiceProxy->musicPlayerExit();
        reply.waitForFinished();
        if (reply.isError()) {
            qDebug() << "method call Multimedia::musicPlayerExit failed" << reply.error();
        }
    }
}

void Multimedia::imagePlayerPlayListViewIndex(const int type, const int index)
{
    initializePrivate();
    if (NULL != m_Private->m_MultimediaServiceProxy) {
        QDBusPendingReply<> reply = m_Private->m_MultimediaServiceProxy->imagePlayerPlayListViewIndex(type, index);
        reply.waitForFinished();
        if (reply.isError()) {
            qDebug() << "method call Multimedia::imagePlayerPlayListViewIndex failed" << reply.error();
        }
    }
}

void Multimedia::imagePlayerPlayPreviousListViewIndex()
{
    initializePrivate();
    if (NULL != m_Private->m_MultimediaServiceProxy) {
        QDBusPendingReply<> reply = m_Private->m_MultimediaServiceProxy->imagePlayerPlayPreviousListViewIndex();
        reply.waitForFinished();
        if (reply.isError()) {
            qDebug() << "method call Multimedia::imagePlayerPlayListViewIndex failed" << reply.error();
        }
    }
}

void Multimedia::imagePlayerPlayNextListViewIndex()
{
    initializePrivate();
    if (NULL != m_Private->m_MultimediaServiceProxy) {
        QDBusPendingReply<> reply = m_Private->m_MultimediaServiceProxy->imagePlayerPlayNextListViewIndex();
        reply.waitForFinished();
        if (reply.isError()) {
            qDebug() << "method call Multimedia::imagePlayerPlayNextListViewIndex failed" << reply.error();
        }
    }
}

void Multimedia::imagePlayerRotateImage()
{
    initializePrivate();
    if (NULL != m_Private->m_MultimediaServiceProxy) {
        QDBusPendingReply<> reply = m_Private->m_MultimediaServiceProxy->imagePlayerRotateImage();
        reply.waitForFinished();
        if (reply.isError()) {
            qDebug() << "method call Multimedia::imagePlayerRotateImage failed" << reply.error();
        }
    }
}

void Multimedia::imagePlayerZoomInImage()
{
    initializePrivate();
    if (NULL != m_Private->m_MultimediaServiceProxy) {
        QDBusPendingReply<> reply = m_Private->m_MultimediaServiceProxy->imagePlayerZoomInImage();
        reply.waitForFinished();
        if (reply.isError()) {
            qDebug() << "method call Multimedia::imagePlayerZoomInImage failed" << reply.error();
        }
    }
}

void Multimedia::imagePlayerZoomOutImage()
{
    initializePrivate();
    if (NULL != m_Private->m_MultimediaServiceProxy) {
        QDBusPendingReply<> reply = m_Private->m_MultimediaServiceProxy->imagePlayerZoomOutImage();
        reply.waitForFinished();
        if (reply.isError()) {
            qDebug() << "method call Multimedia::imagePlayerZoomOutImage failed" << reply.error();
        }
    }
}

void Multimedia::videoPlayerSetPlayModeToggle()
{
    initializePrivate();
    if (NULL != m_Private->m_MultimediaServiceProxy) {
        QDBusPendingReply<void> reply = m_Private->m_MultimediaServiceProxy->videoPlayerSetPlayModeToggle();
        reply.waitForFinished();
        if (reply.isError()) {
            qDebug() << "method call Multimedia::videoPlayerSetPlayModeToggle failed" << reply.error();
        }
    }
}

void Multimedia::videoPlayerSetPlayStatusToggle()
{
    initializePrivate();
    if (NULL != m_Private->m_MultimediaServiceProxy) {
        QDBusPendingReply<void> reply = m_Private->m_MultimediaServiceProxy->videoPlayerSetPlayStatusToggle();
        reply.waitForFinished();
        if (reply.isError()) {
            qDebug() << "method call Multimedia::videoPlayerSetPlayStatusToggle failed" << reply.error();
        }
    }
}

void Multimedia::videoPlayerSetPlayStatus(const VideoPlayerPlayStatus status)
{
    initializePrivate();
    if (NULL != m_Private->m_MultimediaServiceProxy) {
        QDBusPendingReply<void> reply = m_Private->m_MultimediaServiceProxy->videoPlayerSetPlayStatus(status);
        reply.waitForFinished();
        if (reply.isError()) {
            qDebug() << "method call Multimedia::videoPlayerSetPlayStatus failed" << reply.error();
        }
    }
}

void Multimedia::videoPlayerPlayListViewIndex(const int type, const int index, const int x, const int y, const int width, const int height, const int millesimal)
{
    initializePrivate();
    m_Private->requestAudioSource(AS_Video);
    if (NULL != m_Private->m_MultimediaServiceProxy) {
        QDBusPendingReply<void> reply = m_Private->m_MultimediaServiceProxy->videoPlayerPlayListViewIndex(type, index, x, y, width, height, millesimal);
        reply.waitForFinished();
        if (reply.isError()) {
            qDebug() << "method call Multimedia::videoPlayerPlayListViewIndex failed" << reply.error();
        }
    }
}

void Multimedia::videoPlayerPlayPreviousListViewIndex()
{
    initializePrivate();
    if (NULL != m_Private->m_MultimediaServiceProxy) {
        QDBusPendingReply<void> reply = m_Private->m_MultimediaServiceProxy->videoPlayerPlayPreviousListViewIndex();
        reply.waitForFinished();
        if (reply.isError()) {
            qDebug() << "method call Multimedia::videoPlayerPlayPreviousListViewIndex failed" << reply.error();
        }
    }
}

void Multimedia::videoPlayerPlayNextListViewIndex()
{
    initializePrivate();
    if (NULL != m_Private->m_MultimediaServiceProxy) {
        QDBusPendingReply<void> reply = m_Private->m_MultimediaServiceProxy->videoPlayerPlayNextListViewIndex();
        reply.waitForFinished();
        if (reply.isError()) {
            qDebug() << "method call Multimedia::videoPlayerPlayNextListViewIndex failed" << reply.error();
        }
    }
}

void Multimedia::videoPlayerSeekToMillesimal(const int millesimal)
{
    initializePrivate();
    if (NULL != m_Private->m_MultimediaServiceProxy) {
        QDBusPendingReply<void> reply = m_Private->m_MultimediaServiceProxy->videoPlayerSeekToMillesimal(millesimal);
        reply.waitForFinished();
        if (reply.isError()) {
            qDebug() << "method call Multimedia::videoPlayerSeekToMillesimal failed" << reply.error();
        }
    }
}

void Multimedia::videoPlayerExit()
{
    initializePrivate();
    if (NULL != m_Private->m_MultimediaServiceProxy) {
        QDBusPendingReply<> reply = m_Private->m_MultimediaServiceProxy->videoPlayerExit();
        reply.waitForFinished();
        if (reply.isError()) {
            qDebug() << "method call Multimedia::musicPlayerExit failed" << reply.error();
        }
    }
}

Multimedia::ImageFocus Multimedia::getImageFocus()
{
    return m_Private->m_ImageFocus;
}

DeviceWatcherStatus Multimedia::getDeviceStatus(const DeviceWatcherType type)
{
    DeviceWatcherStatus status = DWS_Undefine;
    if (DWT_SDDisk == type) {
        status = m_Private->m_SDStatus;
    } else if (DWT_USBDisk == type) {
        status = m_Private->m_USBStatus;
    }
    return status;
}

DeviceWatcherType Multimedia::getDeviceActiveType()
{
    return m_Private->m_DeviceWatcherType;
}

bool Multimedia::musicListEmpty(const DeviceWatcherType type)
{
    bool flag = true;
    if (DWT_SDDisk == type) {
        flag = m_Private->m_SDDiskMusicListEmpty;
    } else if (DWT_USBDisk == type) {
        flag = m_Private->m_USBDiskMusicListEmpty;
    }
    qDebug() << __PRETTY_FUNCTION__ << __LINE__ << type << flag;
    return flag;
}

bool Multimedia::imageListEmpty(const DeviceWatcherType type)
{
    bool flag = true;
    if (DWT_SDDisk == type) {
        flag = m_Private->m_SDDiskImageListEmpty;
    } else if (DWT_USBDisk == type) {
        flag = m_Private->m_USBDiskImageListEmpty;
    }
    qDebug() << __PRETTY_FUNCTION__ << __LINE__ << type << flag;
    return flag;
}

bool Multimedia::videoListEmpty(const DeviceWatcherType type)
{
    bool flag = true;
    if (DWT_SDDisk == type) {
        flag = m_Private->m_SDDiskVideoListEmpty;
    } else if (DWT_USBDisk == type) {
        flag = m_Private->m_USBDiskVideoListEmpty;
    }
    qDebug() << __PRETTY_FUNCTION__ << __LINE__ << type << flag;
    return flag;
}

void Multimedia::onDeviceWatcherStatusHandler(const DeviceWatcherType type, const DeviceWatcherStatus status)
{
    if (DWT_SDDisk == type) {
        m_Private->m_SDStatus = status;
    } else if (DWT_USBDisk == type) {
        m_Private->m_USBStatus = status;
    }
    if (DWS_Remove == status) {
        if (DWT_SDDisk == type) {
            m_Private->m_SDDiskMusicListEmpty = true;
            m_Private->m_SDDiskImageListEmpty = true;
            m_Private->m_SDDiskVideoListEmpty = true;
        } else if (DWT_USBDisk == type) {
            m_Private->m_USBDiskMusicListEmpty = true;
            m_Private->m_USBDiskImageListEmpty = true;
            m_Private->m_USBDiskVideoListEmpty = true;
        }
        switch (g_Audio->getMultimediaSource()) {
        case AS_Music:
        case AS_Video: {
            if (type == m_Private->m_DeviceWatcherType) {
                m_Private->m_DeviceWatcherType = DWT_Undefine;
                g_Radio->initialize();
            }
            break;
        }
        default: {
            break;
        }
        }
        //        if ((AS_Music == m_Private->m_AudioSource)
        //                || (AS_Video == m_Private->m_AudioSource)) {
        //            if (type == m_Private->m_DeviceWatcherType) {
        //                m_Private->m_DeviceWatcherType = DWT_Undefine;
        //                g_Radio->initialize();
        //            }
        //        }
    }
}

void Multimedia::onMusicPlayerFileNamesHandler(const int type, const QString &xml)
{
    if (DWT_SDDisk == type) {
        QDomDocument document;
        document.setContent(xml);
        QDomElement root = document.documentElement();
        if ((!root.isNull())
                && (root.isElement())
                && (QString("MusicPlayer") == root.toElement().tagName())
                && (root.hasChildNodes())) {
            QDomNode node = root.firstChild();
            while (!node.isNull()) {
                if (node.isElement()) {
                    QDomElement element = node.toElement();
                    if (!element.isNull()) {
                        if (QString("SDFileNames") == element.tagName()) {
                            QDomNodeList nodeList = element.childNodes();
                            if (0 == nodeList.length()) {
                                m_Private->m_SDDiskMusicListEmpty = true;
                            } else {
                                m_Private->m_SDDiskMusicListEmpty = false;
                            }
                            break;
                        }
                    }
                }
                node = node.nextSibling();
            }
        }
    } else if (DWT_USBDisk == type) {
        QDomDocument document;
        document.setContent(xml);
        QDomElement root = document.documentElement();
        if ((!root.isNull())
                && (root.isElement())
                && (QString("MusicPlayer") == root.toElement().tagName())
                && (root.hasChildNodes())) {
            QDomNode node = root.firstChild();
            while (!node.isNull()) {
                if (node.isElement()) {
                    QDomElement element = node.toElement();
                    if (!element.isNull()) {
                        if (QString("USBFileNames") == element.tagName()) {
                            QDomNodeList nodeList = element.childNodes();
                            if (0 == nodeList.length()) {
                                m_Private->m_USBDiskMusicListEmpty = true;
                            } else {
                                m_Private->m_USBDiskMusicListEmpty = false;
                            }
                            break;
                        }
                    }
                }
                node = node.nextSibling();
            }
        }
    }
}

void Multimedia::onImagePlayerFileNamesHandler(const int type, const QString &xml)
{
    if (DWT_SDDisk == type) {
        QDomDocument document;
        document.setContent(xml);
        QDomElement root = document.documentElement();
        if ((!root.isNull())
                && (root.isElement())
                && (QString("ImagePlayer") == root.toElement().tagName())
                && (root.hasChildNodes())) {
            QDomNode node = root.firstChild();
            while (!node.isNull()) {
                if (node.isElement()) {
                    QDomElement element = node.toElement();
                    if (!element.isNull()) {
                        if (QString("SDFileNames") == element.tagName()) {
                            QDomNodeList nodeList = element.childNodes();
                            if (0 == nodeList.length()) {
                                m_Private->m_SDDiskImageListEmpty = true;
                            } else {
                                m_Private->m_SDDiskImageListEmpty = false;
                            }
                            break;
                        }
                    }
                }
                node = node.nextSibling();
            }
        }
    } else if (DWT_USBDisk == type) {
        QDomDocument document;
        document.setContent(xml);
        QDomElement root = document.documentElement();
        if ((!root.isNull())
                && (root.isElement())
                && (QString("ImagePlayer") == root.toElement().tagName())
                && (root.hasChildNodes())) {
            QDomNode node = root.firstChild();
            while (!node.isNull()) {
                if (node.isElement()) {
                    QDomElement element = node.toElement();
                    if (!element.isNull()) {
                        if (QString("USBFileNames") == element.tagName()) {
                            QDomNodeList nodeList = element.childNodes();
                            if (0 == nodeList.length()) {
                                m_Private->m_USBDiskImageListEmpty = true;
                            } else {
                                m_Private->m_USBDiskImageListEmpty = false;
                            }
                            break;
                        }
                    }
                }
                node = node.nextSibling();
            }
        }
    }
}

void Multimedia::onVideoPlayerFileNamesHandler(const int type, const QString &xml)
{
    if (DWT_SDDisk == type) {
        QDomDocument document;
        document.setContent(xml);
        QDomElement root = document.documentElement();
        if ((!root.isNull())
                && (root.isElement())
                && (QString("VideoPlayer") == root.toElement().tagName())
                && (root.hasChildNodes())) {
            QDomNode node = root.firstChild();
            while (!node.isNull()) {
                if (node.isElement()) {
                    QDomElement element = node.toElement();
                    if (!element.isNull()) {
                        if (QString("SDFileNames") == element.tagName()) {
                            QDomNodeList nodeList = element.childNodes();
                            if (0 == nodeList.length()) {
                                m_Private->m_SDDiskVideoListEmpty = true;
                            } else {
                                m_Private->m_SDDiskVideoListEmpty = false;
                            }
                            break;
                        }
                    }
                }
                node = node.nextSibling();
            }
        }
    } else if (DWT_USBDisk == type) {
        QDomDocument document;
        document.setContent(xml);
        QDomElement root = document.documentElement();
        if ((!root.isNull())
                && (root.isElement())
                && (QString("VideoPlayer") == root.toElement().tagName())
                && (root.hasChildNodes())) {
            QDomNode node = root.firstChild();
            while (!node.isNull()) {
                if (node.isElement()) {
                    QDomElement element = node.toElement();
                    if (!element.isNull()) {
                        if (QString("USBFileNames") == element.tagName()) {
                            QDomNodeList nodeList = element.childNodes();
                            if (0 == nodeList.length()) {
                                m_Private->m_USBDiskVideoListEmpty = true;
                            } else {
                                m_Private->m_USBDiskVideoListEmpty = false;
                            }
                            break;
                        }
                    }
                }
                node = node.nextSibling();
            }
        }
    }
}

void Multimedia::onMusicPlayerPlayStatusHandler(const DeviceWatcherType type, const MusicPlayerPlayStatus status)
{
    qDebug() << __PRETTY_FUNCTION__ << type;
    m_Private->m_DeviceWatcherType = type;
}

void Multimedia::onImagePlayerChangeHandler(const int type, const QString &filePath, const int index, const int percent, const int rotate)
{
    qDebug() << __PRETTY_FUNCTION__ << type;
    m_Private->m_DeviceWatcherType = type;
}

void Multimedia::onVideoPlayerPlayStatusHandler(const DeviceWatcherType type, const VideoPlayerPlayStatus status)
{
    qDebug() << __PRETTY_FUNCTION__ << type;
    m_Private->m_DeviceWatcherType = type;
}

void Multimedia::onHolderChange(const AudioSource oldHolder, const AudioSource newHolder)
{
    qDebug() << __PRETTY_FUNCTION__ << oldHolder << newHolder;
    //    if (AS_BluetoothPhone != newHolder) {
    //        m_Private->m_AudioSource = newHolder;
    //    }
    switch (oldHolder) {
    case AS_Music: {
        switch (newHolder) {
        case AS_Radio:
        case AS_BluetoothMusic:
        case AS_BluetoothPhone:
        case AS_CarplayMusic:
        case AS_CarplayPhone:
        case AS_CarlifeMusic:
        case AS_CarlifePhone:
        case AS_AutoMusic:
        case AS_AutoPhone:
        case AS_Aux: {
            musicPlayerSetPlayStatus(MPPS_Pause);
            break;
        }
        default: {
            musicPlayerExit();
            break;
        }
        }
        break;
    }
    case AS_Video: {
        switch (newHolder) {
        case AS_Radio:
        case AS_BluetoothMusic:
        case AS_BluetoothPhone:
        case AS_Aux: {
            videoPlayerSetPlayStatus(VPPS_Pause);
            break;
        }
        default: {
            videoPlayerExit();
            break;
        }
        }
        break;
    }
    default: {
        break;
    }
    }
    switch (newHolder) {
    case AS_Music: {
        qDebug() << __PRETTY_FUNCTION__ << __LINE__;
        videoPlayerExit();
        musicPlayerSetPlayStatus(MPPS_Play);
        musicInitialize();
        break;
    }
    case AS_Video: {
        musicPlayerExit();
        //        videoPlayerSetPlayStatus(VPPS_Play);
        //        videoInitialize();
        break;
    }
    case AS_CarplayMusic:
    case AS_CarplayPhone:
    case AS_AutoMusic:
    case AS_AutoPhone:
    case AS_CarlifeMusic:
    case AS_CarlifePhone: {
        videoPlayerExit();
        break;
    }
    default: {
        break;
    }
    }
}

Multimedia::Multimedia(QObject *parent)
    : QObject(parent)
    , m_Private(NULL)
{
    initializePrivate();
}

Multimedia::~Multimedia()
{
}

void Multimedia::initializePrivate()
{
    if (NULL == m_Private) {
        m_Private.reset(new MultimediaPrivate(this));
    }
}

MultimediaPrivate::MultimediaPrivate(Multimedia *parent)
    : m_Parent(parent)
{
    m_MultimediaServiceProxy = NULL;
    m_ImageFocus = Multimedia::F_ImageUnfocus;
    //    m_AudioSource = AS_Radio;
    m_DeviceWatcherType = DWT_Undefine;
    m_SDStatus = DWS_Empty;
    m_USBStatus = DWS_Empty;
    m_SDDiskMusicListEmpty = true;
    m_USBDiskMusicListEmpty = true;
    m_SDDiskImageListEmpty = true;
    m_USBDiskImageListEmpty = true;
    m_SDDiskVideoListEmpty = true;
    m_USBDiskVideoListEmpty = true;
    initializeProxy();
    connectAllSlots();
}

MultimediaPrivate::~MultimediaPrivate()
{
}

void MultimediaPrivate::initializeProxy()
{
    if (NULL == m_MultimediaServiceProxy) {
        m_MultimediaServiceProxy = new Local::DbusServer::Multimedia(ArkMicroMultimediaService,
                                                                     ArkMicroMultimediaPath,
                                                                     QDBusConnection::sessionBus(),
                                                                     qApp);
    }
}

bool MultimediaPrivate::requestAudioSource(const AudioSource source)
{
    switch (source) {
    case AS_Music:
    case AS_Video: {
        g_Audio->requestAudioSource(source);
        break;
    }
    default: {
        break;
    }
    }
}

void MultimediaPrivate::releaseAudioSource(const AudioSource source)
{
    switch (source) {
    case AS_Music:
    case AS_Video: {
        g_Audio->releaseAudioSource(source);
    }
    default: {
        break;
    }
    }
}

void MultimediaPrivate::connectAllSlots()
{
    if (NULL != m_MultimediaServiceProxy) {
        connectSignalAndSignalByNamesake(m_MultimediaServiceProxy, m_Parent, ARKSENDER(onDeviceWatcherStatus(int, int)));
        connectSignalAndSignalByNamesake(m_MultimediaServiceProxy, m_Parent, ARKSENDER(onImagePlayerChange(int, const QString &, int, int, int)));
        connectSignalAndSignalByNamesake(m_MultimediaServiceProxy, m_Parent, ARKSENDER(onImagePlayerFileNames(int, const QString &)));
        connectSignalAndSignalByNamesake(m_MultimediaServiceProxy, m_Parent, ARKSENDER(onMusicPlayerElapsedInformation(int, int)));
        connectSignalAndSignalByNamesake(m_MultimediaServiceProxy, m_Parent, ARKSENDER(onMusicPlayerFileNames(int, const QString &)));
        connectSignalAndSignalByNamesake(m_MultimediaServiceProxy, m_Parent, ARKSENDER(onMusicPlayerID3TagChange(int, int, const QString , const QString, const QString &, const QString &, int)));
        connectSignalAndSignalByNamesake(m_MultimediaServiceProxy, m_Parent, ARKSENDER(onMusicPlayerPlayMode(int)));
        connectSignalAndSignalByNamesake(m_MultimediaServiceProxy, m_Parent, ARKSENDER(onMusicPlayerPlayStatus(int, int)));
        connectSignalAndSignalByNamesake(m_MultimediaServiceProxy, m_Parent, ARKSENDER(onVideoPlayerElapsedInformation(int, int)));
        connectSignalAndSignalByNamesake(m_MultimediaServiceProxy, m_Parent, ARKSENDER(onVideoPlayerFileNames(int, const QString &)));
        connectSignalAndSignalByNamesake(m_MultimediaServiceProxy, m_Parent, ARKSENDER(onVideoPlayerInformation(int, int, const QString &, int)));
        connectSignalAndSignalByNamesake(m_MultimediaServiceProxy, m_Parent, ARKSENDER(onVideoPlayerPlayMode(int)));
        connectSignalAndSignalByNamesake(m_MultimediaServiceProxy, m_Parent, ARKSENDER(onVideoPlayerPlayStatus(int, int)));
        connectSignalAndSignalByNamesake(m_MultimediaServiceProxy, m_Parent, ARKSENDER(onVideoPlayerShowStatus(int)));
        connectSignalAndSlotByNamesake(g_Audio, m_Parent, ARKRECEIVER(onHolderChange(const int, const int)));
        Qt::ConnectionType type = static_cast<Qt::ConnectionType>(Qt::AutoConnection | Qt::UniqueConnection);
        QObject::connect(m_MultimediaServiceProxy, ARKSENDER(onDeviceWatcherStatus(const int, const int)),
                         m_Parent,                 ARKRECEIVER(onDeviceWatcherStatusHandler(const int, const int)),
                         type);
        QObject::connect(m_MultimediaServiceProxy, ARKSENDER(onMusicPlayerFileNames(const int, const QString &)),
                         m_Parent,                 ARKRECEIVER(onMusicPlayerFileNamesHandler(const int, const QString &)),
                         type);
        QObject::connect(m_MultimediaServiceProxy, ARKSENDER(onImagePlayerFileNames(const int, const QString &)),
                         m_Parent,                 ARKRECEIVER(onImagePlayerFileNamesHandler(const int, const QString &)),
                         type);
        QObject::connect(m_MultimediaServiceProxy, ARKSENDER(onVideoPlayerFileNames(const int, const QString &)),
                         m_Parent,                 ARKRECEIVER(onVideoPlayerFileNamesHandler(const int, const QString &)),
                         type);
        QObject::connect(m_MultimediaServiceProxy, ARKSENDER(onMusicPlayerPlayStatus(const int, const int)),
                         m_Parent,                 ARKRECEIVER(onMusicPlayerPlayStatusHandler(const int, const int)),
                         type);
        QObject::connect(m_MultimediaServiceProxy, ARKSENDER(onImagePlayerChange(const int, const QString &, const int, const int, const int)),
                         m_Parent,                 ARKRECEIVER(onImagePlayerChangeHandler(const int, const QString &, const int, const int, const int)),
                         type);
        QObject::connect(m_MultimediaServiceProxy, ARKSENDER(onVideoPlayerPlayStatus(const int, const int)),
                         m_Parent,                 ARKRECEIVER(onVideoPlayerPlayStatusHandler(const int, const int)),
                         type);
    }
}
