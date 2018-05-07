#include "DeviceWatcher.h"
#include "DiskScanner/DiskScanner.h"
#include "DiskDetach/DiskDetach.h"
#include "RunnableThread.h"
#include <QDebug>
#include "Detach.h"
#include <QDir>
#include <QRegExp>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <QTimer>

static const QString mountUSBPath("/media/udisk");
static const QString mountSDPath("/media/sdisk");

class DeviceWatcherPrivate
{
    Q_DISABLE_COPY(DeviceWatcherPrivate)
public:
    explicit DeviceWatcherPrivate(DeviceWatcher *parent);
    ~DeviceWatcherPrivate();
    void initialize();
    void connectAllSlots();
    void diskDeviceAdd(const QString &path);
    void diskDeviceRemove(const QString &path);
    void usbDiskScan(const QString &path);
    void usbDiskCancelScan(const QString &path);
    void sdDiskScan(const QString &path);
    void sdDiskCancelScan(const QString &path);
    bool regExpUSBPath(const QString& path);
    bool regExpSDPath(const QString& path);
    DiskScanner* m_USBDiskScanner;
    DiskScanner* m_SDDiskScanner;
    DiskDetach* m_DiskDetach;
    QString m_USBDiskPath;
    QString m_SDDiskPath;
    DeviceWatcherStatus m_USBDiskStatus;
    DeviceWatcherStatus m_SDDiskStatus;
    bool m_InitDetach;
private:
    DeviceWatcher* m_Parent;
};

DeviceWatcher::DeviceWatcher(QObject *parent)
    : QObject(parent)
    , m_Private(new DeviceWatcherPrivate(this))
{
}

DeviceWatcher::~DeviceWatcher()
{
}

void DeviceWatcher::onDiskDeviceChangeHandler(const QString &device, const DiskDeviceWatcher::Action action)
{
    m_Private->m_DiskDetach->onDiskDeviceChange(device, action);
}

void DeviceWatcher::synchronize()
{
    deviceWatcherCheckStatus(DWT_SDDisk);
    deviceWatcherCheckStatus(DWT_USBDisk);
}

void DeviceWatcher::deviceWatcherCheckStatus(const int type)
{
    switch (type) {
    case DWT_SDDisk: {
        emit onDeviceWatcherStatus(DWT_SDDisk, m_Private->m_SDDiskStatus);
        break;
    }
    case DWT_USBDisk: {
        emit onDeviceWatcherStatus(DWT_USBDisk, m_Private->m_USBDiskStatus);
        break;
    }
    default: {
        break;
    }
    }
}
void DeviceWatcher::timerEvent(QTimerEvent *event)
{
    killTimer(event->timerId());
    if (!QString(qgetenv("PROTOCOL_ID")).contains(QString("ark169"))
            && !QString(qgetenv("PROTOCOL_ID")).contains(QString("yaoxi"))) {
        system("insmod /lib/modules/3.4.0/kernel/drivers/ark/sdmmc/ark_dw_mmc.ko");
        system("insmod /lib/modules/3.4.0/kernel/drivers/usb/musb/musb_hdrc.ko");
        system("insmod /lib/modules/3.4.0/kernel/drivers/usb/musb/ark1680_musb.ko");
        system("insmod /lib/modules/3.4.0/kernel/drivers/usb/gadget/g_ncm.ko");
        system("echo otg > /sys/devices/platform/musb-ark1680.0/musb-hdrc.0/mode");
    } else {
        QDir dir("/dev/");
        dir.setFilter(QDir::System | QDir::NoDotAndDotDot);
        QFileInfoList fileInfoList = dir.entryInfoList();
        bool sdExsits(false);
        bool usbExists(false);
        for (QFileInfoList::iterator fileIter = fileInfoList.begin(); fileIter != fileInfoList.end(); ++fileIter) {
            if ((!sdExsits) && (m_Private->regExpSDPath(fileIter->filePath()))) {
                sdExsits = true;
                onDiskDeviceChangeHandler(fileIter->filePath(), DiskDeviceWatcher::A_Add);
            } else if ((!usbExists) && (m_Private->regExpUSBPath(fileIter->filePath()))) {
                usbExists = true;
                if (QString(qgetenv("PROTOCOL_ID")).contains(QString("yaoxi"))) {
                    system("echo otg > /sys/devices/platform/musb-ark1680.1/musb-hdrc.1/mode");
                } else {
                    onDiskDeviceChangeHandler(fileIter->filePath(), DiskDeviceWatcher::A_Add);
                }
            } else if (sdExsits && usbExists) {
                break;
            }
        }
    }
    QObject::timerEvent(event);
}

DeviceWatcherPrivate::DeviceWatcherPrivate(DeviceWatcher *parent)
    : m_Parent(parent)
{
    m_USBDiskScanner = NULL;
    m_SDDiskScanner = NULL;
    m_DiskDetach = NULL;
    m_USBDiskStatus = DWS_Empty;
    m_SDDiskStatus = DWS_Empty;
    m_InitDetach = false;
    initialize();
    connectAllSlots();
    m_Parent->startTimer(0);
}

DeviceWatcherPrivate::~DeviceWatcherPrivate()
{
    if (m_InitDetach) {
      //  uninitDetach();
        m_InitDetach = false;
    }
}

void DeviceWatcherPrivate::initialize()
{
    QStringList m_MusicSuffix;
    m_MusicSuffix.clear();
    m_MusicSuffix.append(QString(".MP2"));
    m_MusicSuffix.append(QString(".MP3"));
    m_MusicSuffix.append(QString(".WMA"));
    m_MusicSuffix.append(QString(".M4A"));
    m_MusicSuffix.append(QString(".FLAC"));
    m_MusicSuffix.append(QString(".APE"));
    m_MusicSuffix.append(QString(".OGG"));
    m_MusicSuffix.append(QString(".AC3"));
    m_MusicSuffix.append(QString(".AAC"));
    m_MusicSuffix.append(QString(".WAV"));
    m_MusicSuffix.append(QString(".AMR"));
    m_MusicSuffix.append(QString(".RA"));
    m_MusicSuffix.append(QString(".AU"));
    m_MusicSuffix.append(QString(".MMF"));

    QStringList m_ImageSuffix;
    m_ImageSuffix.clear();
    m_ImageSuffix.append(QString(".JPG"));
    m_ImageSuffix.append(QString(".BMP"));
    m_ImageSuffix.append(QString(".JPEG"));
    m_ImageSuffix.append(QString(".PNG"));
    m_ImageSuffix.append(QString(".GIF"));
    QStringList m_VideoSuffix;
    m_VideoSuffix.clear();
    m_VideoSuffix.append(QString(".AVI"));
    m_VideoSuffix.append(QString(".MP4"));
    m_VideoSuffix.append(QString(".MPG"));
    m_VideoSuffix.append(QString(".M4V"));
    m_VideoSuffix.append(QString(".MKV"));
    m_VideoSuffix.append(QString(".3GP"));
    m_VideoSuffix.append(QString(".ASF"));
    m_VideoSuffix.append(QString(".MOV"));
    m_VideoSuffix.append(QString(".MPEG"));
    m_VideoSuffix.append(QString(".VOB"));
    m_VideoSuffix.append(QString(".TS"));
    m_VideoSuffix.append(QString(".WMV"));
    m_VideoSuffix.append(QString(".RM"));
    m_VideoSuffix.append(QString(".RMVB"));
    m_VideoSuffix.append(QString(".DIVX"));
    m_VideoSuffix.append(QString(".FLV"));
    m_VideoSuffix.append(QString(".SWF"));
    m_VideoSuffix.append(QString(".OGM"));
    m_VideoSuffix.append(QString(".DAT"));

    m_USBDiskScanner = new DiskScanner(m_Parent);
    m_USBDiskScanner->setMinimumScanTime(1500);
    QMap<int, QStringList> map;
    map.insert(0, m_MusicSuffix);
    map.insert(1, m_ImageSuffix);
    map.insert(2, m_VideoSuffix);
    m_USBDiskScanner->setScannerSuffixMap(map);

    m_SDDiskScanner = new DiskScanner(m_Parent);
    m_SDDiskScanner->setMinimumScanTime(1500);
    map.insert(0, m_MusicSuffix);
    map.insert(1, m_ImageSuffix);
    map.insert(2, m_VideoSuffix);
    m_SDDiskScanner->setScannerSuffixMap(map);

    m_USBDiskPath.clear();
    m_SDDiskPath.clear();

    m_DiskDetach = new DiskDetach(m_Parent);
}

void DeviceWatcherPrivate::connectAllSlots()
{
    Qt::ConnectionType type = static_cast<Qt::ConnectionType>(Qt::UniqueConnection | Qt::AutoConnection);
    QObject::connect(m_USBDiskScanner, SIGNAL(filePath(const QString &, const int)),
                     m_Parent,         SLOT(onUSBFilePath(const QString &, const int)),
                     type);
    QObject::connect(m_USBDiskScanner, SIGNAL(finish()),
                     m_Parent,         SLOT(onFinish()),
                     type);
    QObject::connect(m_SDDiskScanner, SIGNAL(filePath(const QString &, const int)),
                     m_Parent,        SLOT(onSDFilePath(const QString &, const int)),
                     type);
    QObject::connect(m_SDDiskScanner, SIGNAL(finish()),
                     m_Parent,        SLOT(onFinish()),
                     type);
    QObject::connect(m_DiskDetach, SIGNAL(diskPartition(const QString &, const DiskDeviceWatcher::Action)),
                     m_Parent,     SLOT(onDiskDeviceChange(const QString &, const DiskDeviceWatcher::Action)),
                     type);
}

void DeviceWatcherPrivate::diskDeviceAdd(const QString &path)
{
    if (regExpUSBPath(path)) {
        if (m_USBDiskPath.isEmpty()
                && QFile::exists(path)) {
            if (!QFile::exists(mountUSBPath)) {
                system("mkdir -p /media/udisk");
            }
            if (!m_InitDetach) {
               // initDetach();
                m_InitDetach = true;
            }
           // const QString temp(detach(path.toLocal8Bit().constData()));
            const QString temp("");
            QString argument(" ");
            if (temp.contains(QString("FAT (32 bit)"))) {
                argument = QString(" -o utf8 ");
            } else if (temp.contains(QString("FAT (16 bit)"))) {
                argument = QString(" -o utf8 ");
            } else if (temp.contains(QString("ext4"))) {
            } else if (temp.contains(QString("ext3"))) {
            } else if (temp.contains(QString("ext2"))) {
            }/* else if (temp.contains(QString("NTFS"))) {
            }*/ else {
                qDebug() << "unknow type";
                qDebug() << temp;
                qDebug() << "m_USBDiskPath" << m_USBDiskPath;
                m_USBDiskStatus = DWS_Unsupport;
                m_USBDiskPath = path;
                emit m_Parent->onDeviceWatcherStatus(DWT_USBDisk, m_USBDiskStatus);
                return;
            }
            QString command = QString(" mount ") + path + argument + mountUSBPath;
            int ret = system(command.toLocal8Bit().constData());
            if (0 == ret) {
                m_USBDiskPath = path;
                usbDiskScan(mountUSBPath);
            } else {
                qDebug() << path << "command" << command;
                qDebug() << "mount usb failed!!!!!1";
            }
        }
    } else if (regExpSDPath(path)) {
        if (m_SDDiskPath.isEmpty()
                && QFile::exists(path)) {
            if (!QFile::exists(mountSDPath)) {
                system("mkdir -p /media/sdisk");
            }
            if (!m_InitDetach) {
               // initDetach();
                m_InitDetach = true;
            }
          //  const QString temp(detach(path.toLocal8Bit().constData()));
            const QString temp("");
            QString argument(" ");
            if (temp.contains(QString("FAT (32 bit)"))) {
                argument = QString(" -o utf8 ");
            } else if (temp.contains(QString("FAT (16 bit)"))) {
                argument = QString(" -o utf8 ");
            } else if (temp.contains(QString("ext4"))) {
            } else if (temp.contains(QString("ext3"))) {
            } else if (temp.contains(QString("ext2"))) {
            }/* else if (temp.contains(QString("NTFS"))) {
            }*/ else {
                qDebug() << "unknow type";
                qDebug() << temp;
                qDebug() << "m_SDDiskPath" << m_SDDiskPath;
                m_SDDiskStatus = DWS_Unsupport;
                m_SDDiskPath = path;
                emit m_Parent->onDeviceWatcherStatus(DWT_SDDisk, m_SDDiskStatus);
                return;
            }
            QString command = QString(" mount ") + path + argument + mountSDPath;
            int ret = system(command.toLocal8Bit().constData());
            if (0 == ret) {
                m_SDDiskPath = path;
                sdDiskScan(mountSDPath);
            } else {
                qDebug() << path << "command" << command;
                qDebug() << "mount sd failed!!!!!1";
            }
        }
    } else {
        qDebug() << "add other device!";
    }
}

void DeviceWatcherPrivate::diskDeviceRemove(const QString &path)
{
    if (m_USBDiskPath == path) {
        if (DWS_Unsupport == m_USBDiskStatus) {
            m_USBDiskStatus = DWS_Remove;
            m_USBDiskPath.clear();
            emit m_Parent->onDeviceWatcherStatus(DWT_USBDisk, m_USBDiskStatus);
            m_USBDiskStatus = DWS_Empty;
            return ;
        }
        usbDiskCancelScan(mountUSBPath);
        QString command = QString(" umount -l ") + mountUSBPath;
        int ret = system(command.toLocal8Bit().constData());
        if (0 == ret) {
        } else {
            qDebug() << "umount usb failed!!!!";
        }
    } else if (m_SDDiskPath == path) {
        if (DWS_Unsupport == m_SDDiskStatus) {
            m_SDDiskStatus = DWS_Remove;
            m_SDDiskPath.clear();
            emit m_Parent->onDeviceWatcherStatus(DWT_SDDisk, m_SDDiskStatus);
            m_SDDiskStatus = DWS_Empty;
            return ;
        }
        sdDiskCancelScan(mountSDPath);
        QString command = QString(" umount -l ") + mountSDPath;
        int ret = system(command.toLocal8Bit().constData());
        if (0 == ret) {
        } else {
            qDebug() << "umount sd failed!!!!";
        }
    } else {
        qDebug() << "remove other device!" << path << m_SDDiskPath << m_USBDiskPath;
    }
}

void DeviceWatcherPrivate::usbDiskScan(const QString &path)
{
    m_USBDiskStatus = DWS_Busy;
    m_USBDiskScanner->startScanner(path);
    emit m_Parent->onDeviceWatcherStatus(DWT_USBDisk, m_USBDiskStatus);
}

void DeviceWatcherPrivate::usbDiskCancelScan(const QString &path)
{
    m_USBDiskStatus = DWS_Remove;
    m_USBDiskPath.clear();
    m_USBDiskScanner->stopScanner();
    emit m_Parent->onDeviceWatcherStatus(DWT_USBDisk, m_USBDiskStatus);
    m_USBDiskStatus = DWS_Empty;
}

void DeviceWatcherPrivate::sdDiskScan(const QString &path)
{
    m_SDDiskStatus = DWS_Busy;
    m_SDDiskScanner->startScanner(path);
    emit m_Parent->onDeviceWatcherStatus(DWT_SDDisk, m_SDDiskStatus);
}

void DeviceWatcherPrivate::sdDiskCancelScan(const QString &path)
{
    m_SDDiskStatus = DWS_Remove;
    m_SDDiskPath.clear();
    m_SDDiskScanner->stopScanner();
    emit m_Parent->onDeviceWatcherStatus(DWT_SDDisk, m_SDDiskStatus);
    m_SDDiskStatus = DWS_Empty;
}

bool DeviceWatcherPrivate::regExpUSBPath(const QString &path)
{
    return path.startsWith(QString("/dev/sd"));
}


bool DeviceWatcherPrivate::regExpSDPath(const QString &path)
{
    return (path.startsWith(QString("/dev/mmcblk")) && 1/*path.contains(QChar('p'))*/);
}

void DeviceWatcher::onDiskDeviceChange(const QString &device, const DiskDeviceWatcher::Action action)
{
    switch (action) {
    case DiskDeviceWatcher::A_Add: {
        m_Private->diskDeviceAdd(device);
        break;
    }
    case DiskDeviceWatcher::A_Remove: {
        m_Private->diskDeviceRemove(device);
        break;
    }
    default: {
        break;
    }
    }
}

void DeviceWatcher::onUSBFilePath(const QString &path, const int index)
{
    switch(index) {
    case 0: {
        emit onMusicFilePath(path, DWT_USBDisk);
        break;
    }
    case 1: {
        emit onImageFilePath(path, DWT_USBDisk);
        break;
    }
    case 2: {
        emit onVideoFilePath(path, DWT_USBDisk);
        break;
    }
    default: {
        break;
    }
    }
}

void DeviceWatcher::onSDFilePath(const QString &path, const int index)
{
    switch(index) {
    case 0: {
        emit onMusicFilePath(path, DWT_SDDisk);
        break;
    }
    case 1: {
        emit onImageFilePath(path, DWT_SDDisk);
        break;
    }
    case 2: {
        emit onVideoFilePath(path, DWT_SDDisk);
        break;
    }
    default: {
        break;
    }
    }
}

void DeviceWatcher::onFinish()
{
    QObject* ptr = sender();
    if (ptr == m_Private->m_USBDiskScanner) {
        qDebug() << "USB::onFinish";
        m_Private->m_USBDiskStatus = DWS_Ready;
        emit onDeviceWatcherStatus(DWT_USBDisk, m_Private->m_USBDiskStatus);
    } else if (ptr == m_Private->m_SDDiskScanner) {
        qDebug() << "SD::onFinish";
        m_Private->m_SDDiskStatus = DWS_Ready;
        emit onDeviceWatcherStatus(DWT_SDDisk, m_Private->m_SDDiskStatus);
    }
}
