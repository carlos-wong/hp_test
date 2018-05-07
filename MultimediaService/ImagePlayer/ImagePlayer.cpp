#include "ImagePlayer.h"
#include "AutoConnect.h"
#include "Utility.h"
#include "MultimediaPersistent.h"
#include "UserInterfaceUtility.h"
#include <QList>
#include <QFileInfo>
#include <QTimer>
#include <QMatrix>
#include <QImage>
#include <QImageReader>
#include <QBuffer>
#include <QDomDocument>
#include <QDebug>

class ImagePlayerPrivate
{
    Q_DISABLE_COPY(ImagePlayerPrivate)
public:
    explicit ImagePlayerPrivate(ImagePlayer* parent);
    ~ImagePlayerPrivate();
    void initialize();
    void connectAllSlots();
    void createUSBFileNamesXml();
    void createSDFileNamesXml();
    void playListViewIndex(const DeviceWatcherType type, const int index);
    void playPreviousListViewIndex();
    void playNextListViewIndex();
    void rotateImage();
    void zoomInImage();
    void zoomOutImage();
    QByteArray& convertByteArray(const QImage &image);
    QList<QString>& getPathList(const DeviceWatcherType type);
    void exitImagePlayer();
    QList<QString> m_USBPathList;
    QList<QString> m_SDPathList;
    int m_PlayIndex;
    DeviceWatcherType m_DiskType;
    QString m_USBFileNamesXml;
    QString m_SDFileNamesXml;
    QString m_CurrentPath;
    int m_RotateAngle;
    float m_Scale;
private:
    ImagePlayer* m_Parent;
};

ImagePlayer::ImagePlayer(QObject *parent)
    : QObject(parent)
    , m_Private(new ImagePlayerPrivate(this))
{
}

ImagePlayer::~ImagePlayer()
{
}

void ImagePlayer::imagePlayerRequestFileNames(const DeviceWatcherType type)
{
    if (DWT_SDDisk == type) {
        emit onImagePlayerFileNames(DWT_SDDisk, m_Private->m_SDFileNamesXml);
    } else if (DWT_USBDisk == type) {
        emit onImagePlayerFileNames(DWT_USBDisk, m_Private->m_USBFileNamesXml);
    }
}

void ImagePlayer::imagePlayerPlayListViewIndex(const DeviceWatcherType type, const int index)
{
    m_Private->playListViewIndex(type, index);
}

void ImagePlayer::imagePlayerPlayPreviousListViewIndex()
{
    m_Private->playPreviousListViewIndex();
}

void ImagePlayer::imagePlayerPlayNextListViewIndex()
{
    m_Private->playNextListViewIndex();
}

void ImagePlayer::imagePlayerRotateImage()
{
    m_Private->rotateImage();
}

void ImagePlayer::imagePlayerZoomInImage()
{
    m_Private->zoomInImage();
}

void ImagePlayer::imagePlayerZoomOutImage()
{
    m_Private->zoomOutImage();
}

void ImagePlayer::onDeviceWatcherStatus(const DeviceWatcherType type, const DeviceWatcherStatus status)
{
    if (DWT_SDDisk == type) {
        switch (status) {
        case DWS_Empty: {
            m_Private->m_SDPathList.clear();
            break;
        }
        case DWS_Busy: {
            m_Private->m_SDPathList.clear();
            break;
        }
        case DWS_Ready: {
            m_Private->createSDFileNamesXml();
            imagePlayerRequestFileNames(DWT_SDDisk);
            break;
        }
        case DWS_Remove: {
            m_Private->m_SDPathList.clear();
            break;
        }
        default : {
            break;
        }
        }
    } else if (DWT_USBDisk == type) {
        switch (status) {
        case DWS_Empty: {
            m_Private->m_USBPathList.clear();
            break;
        }
        case DWS_Busy: {
            m_Private->m_USBPathList.clear();
            break;
        }
        case DWS_Ready: {
            m_Private->createUSBFileNamesXml();
            imagePlayerRequestFileNames(DWT_USBDisk);
            break;
        }
        case DWS_Remove: {
            m_Private->m_USBPathList.clear();
            break;
        }
        default : {
            break;
        }
        }
    }
}

void ImagePlayer::onImageFilePath(const QString &path, const DeviceWatcherType type)
{    
    if (DWT_SDDisk == type) {
        m_Private->m_SDPathList.append(path);
    } else if (DWT_USBDisk == type) {
        m_Private->m_USBPathList.append(path);
    }
}

ImagePlayerPrivate::ImagePlayerPrivate(ImagePlayer *parent)
    : m_Parent(parent)
{
    m_PlayIndex = 0;
    m_DiskType = DWT_Undefine;
    m_RotateAngle = 0;
    m_Scale = 1.0f;
    initialize();
    connectAllSlots();
}

ImagePlayerPrivate::~ImagePlayerPrivate()
{
}

void ImagePlayerPrivate::initialize()
{
}

void ImagePlayerPrivate::connectAllSlots()
{
    connectSignalAndSlotByNamesake(g_DeviceWatcher, m_Parent, SLOT(onDeviceWatcherStatus(const int, const int)));
    connectSignalAndSlotByNamesake(g_DeviceWatcher, m_Parent, SLOT(onImageFilePath(const QString &, const int)));
}

void ImagePlayerPrivate::createUSBFileNamesXml()
{
    QDomDocument domDocument;
    domDocument.clear();
    domDocument.appendChild(domDocument.createElement(QString("ImagePlayer")));
    QDomElement root = domDocument.firstChildElement(QString("ImagePlayer"));
    QDomElement fileNames;
    QDomElement info;
    fileNames = domDocument.createElement(QString("USBFileNames"));
    root.appendChild(fileNames);
    m_USBFileNamesXml.clear();
    QString usbPersistantPathInfo = MultimediaPersistent::getImagePathInfo(DWT_USBDisk);
    QStringList stringList = usbPersistantPathInfo.split(QChar('/'));
    QString usbPersistantPath = usbPersistantPathInfo.left(usbPersistantPathInfo.length() - stringList.last().length() - 1);
    QString persistantIndex("");
    QFileInfo fileInfo;
    int i;
    for (i = 0; i < m_USBPathList.size(); ++i) {
        info = domDocument.createElement(QString("Index:" + QString::number(i)));
        fileNames.appendChild(info);
        fileInfo.setFile(m_USBPathList.at(i));
        info.appendChild(domDocument.createTextNode(fileInfo.fileName()));
        if (persistantIndex.isEmpty()) {
            if (fileInfo.filePath() == usbPersistantPath) {
                if (usbPersistantPathInfo == (fileInfo.filePath() + QString("/") + fileInfo.created().toString(QString("yyyyMMddhhmmss")) + fileInfo.lastModified().toString(QString("yyyyMMddhhmmss")) + QString::number(fileInfo.size()))) {
                    persistantIndex = QString::number(i);
                }
            }
        }
    }
    QDomElement persistant = domDocument.createElement(QString("USBPersistant"));
    root.appendChild(persistant);
    unsigned char multimediaType = MultimediaPersistent::getMultimediaType(DWT_USBDisk);
    if (persistantIndex.isEmpty()) {
        if (0 == i) {
            persistantIndex = QString("-1");
            multimediaType = MT_Idle;
            MultimediaPersistent::setMultimediaType(DWT_USBDisk, MT_Video);
        } else {
            if (0 == multimediaType) {
                multimediaType = MT_Image;
                MultimediaPersistent::setMultimediaType(DWT_USBDisk, multimediaType);
            }
            persistantIndex = QString("0");
        }
    }
    persistant.appendChild(domDocument.createTextNode(persistantIndex));
    QDomElement type = domDocument.createElement(QString("USBType"));
    root.appendChild(type);
    type.appendChild(domDocument.createTextNode(QString::number(multimediaType)));
    m_USBFileNamesXml = domDocument.toString();
}

void ImagePlayerPrivate::createSDFileNamesXml()
{
    QDomDocument domDocument;
    domDocument.clear();
    domDocument.appendChild(domDocument.createElement(QString("ImagePlayer")));
    QDomElement root = domDocument.firstChildElement(QString("ImagePlayer"));
    QDomElement fileNames;
    QDomElement info;
    fileNames = domDocument.createElement(QString("SDFileNames"));
    root.appendChild(fileNames);
    m_SDFileNamesXml.clear();
    QString sdPersistantPathInfo = MultimediaPersistent::getImagePathInfo(DWT_SDDisk);
    QStringList stringList = sdPersistantPathInfo.split(QChar('/'));
    QString sdPersistantPath = sdPersistantPathInfo.left(sdPersistantPathInfo.length() - stringList.last().length() - 1);
    QString persistantIndex("");
    QFileInfo fileInfo;
    int i;
    for (i = 0; i < m_SDPathList.size(); ++i) {
        info = domDocument.createElement(QString("Index:" + QString::number(i)));
        fileNames.appendChild(info);
        fileInfo.setFile(m_SDPathList.at(i));
        info.appendChild(domDocument.createTextNode(fileInfo.fileName()));
        if (persistantIndex.isEmpty()) {
            if (fileInfo.filePath() == sdPersistantPath) {
                if (sdPersistantPathInfo == (fileInfo.filePath() + QString("/") + fileInfo.created().toString(QString("yyyyMMddhhmmss")) + fileInfo.lastModified().toString(QString("yyyyMMddhhmmss")) + QString::number(fileInfo.size()))) {
                    persistantIndex = QString::number(i);
                }
            }
        }
    }
    QDomElement persistant = domDocument.createElement(QString("SDPersistant"));
    root.appendChild(persistant);
    unsigned char multimediaType = MultimediaPersistent::getMultimediaType(DWT_SDDisk);
    if (persistantIndex.isEmpty()) {
        if (0 == i) {
            persistantIndex = QString("-1");
            multimediaType = MT_Idle;
            MultimediaPersistent::setMultimediaType(DWT_SDDisk, MT_Video);
        } else {
            if (0 == multimediaType) {
                multimediaType = MT_Image;
                MultimediaPersistent::setMultimediaType(DWT_SDDisk, multimediaType);
            }
            persistantIndex = QString("0");
        }
    }
    persistant.appendChild(domDocument.createTextNode(persistantIndex));
    QDomElement type = domDocument.createElement(QString("SDType"));
    root.appendChild(type);
    type.appendChild(domDocument.createTextNode(QString::number(multimediaType)));
    m_SDFileNamesXml = domDocument.toString();
}

void ImagePlayerPrivate::playListViewIndex(const DeviceWatcherType type, const int index)
{
     qDebug()<< "============5============void ImagePlayerPrivate::playListViewIndex()";
    QList<QString> temp = getPathList(type);
    if ((temp.size() > index)
            && (QFile(temp.at(index))).exists()) {
        m_DiskType = type;
        m_RotateAngle = 0;
        m_Scale = 1.0f;
        m_PlayIndex = index;
        m_CurrentPath = temp.at(index);
        int percent = m_Scale * 100;
        emit m_Parent->onImagePlayerChange(m_DiskType, m_CurrentPath, m_PlayIndex, percent, m_RotateAngle);
        QFileInfo fileInfo(m_CurrentPath);
        MultimediaPersistent::setImagePathInfo(m_DiskType, fileInfo.filePath() + QString("/") + fileInfo.created().toString(QString("yyyyMMddhhmmss")) + fileInfo.lastModified().toString(QString("yyyyMMddhhmmss")) + QString::number(fileInfo.size()));
        MultimediaPersistent::setMultimediaType(m_DiskType, MT_Image);
    }
}

void ImagePlayerPrivate::playPreviousListViewIndex()
{
    int lastIndex = m_PlayIndex;
    QList<QString> temp = getPathList(m_DiskType);
    if ((lastIndex > 0)
            && (lastIndex <= temp.size() - 1)) {
        --lastIndex;
    } else {
        lastIndex = temp.size() - 1;
    }
    playListViewIndex(m_DiskType, lastIndex);
}

void ImagePlayerPrivate::playNextListViewIndex()
{
    qDebug()<< "============4============void ImagePlayerPrivate::playNextListViewIndex()";
    m_RotateAngle = 0;
    int lastIndex = m_PlayIndex;
    QList<QString> temp = getPathList(m_DiskType);
    if (((lastIndex < (temp.size() - 1)))
            && (lastIndex >= 0)) {
        ++lastIndex;
    } else {
        lastIndex = 0;
    }
    playListViewIndex(m_DiskType, lastIndex);
}

void ImagePlayerPrivate::rotateImage()
{
    int step(90);
    if (((m_RotateAngle + step) < 360)
            && (m_RotateAngle >= 0)) {
        m_RotateAngle += step;
    } else {
        m_RotateAngle = 0;
    }
    int percent = m_Scale * 100;
    emit m_Parent->onImagePlayerChange(m_DiskType, m_CurrentPath, m_PlayIndex, percent, m_RotateAngle);
}

void ImagePlayerPrivate::zoomInImage()
{
    if (m_Scale > 1.0f) {
        m_Scale -= 0.25f;
    }
    int percent = m_Scale * 100;
    emit m_Parent->onImagePlayerChange(m_DiskType, m_CurrentPath, m_PlayIndex, percent, m_RotateAngle);
}

void ImagePlayerPrivate::zoomOutImage()
{
    if (m_Scale < 2.5f) {
        m_Scale += 0.25f;
    }
    int percent = m_Scale * 100;
    emit m_Parent->onImagePlayerChange(m_DiskType, m_CurrentPath, m_PlayIndex, percent, m_RotateAngle);
}

QByteArray &ImagePlayerPrivate::convertByteArray(const QImage &image)
{
    QByteArray byteArray;
    return  byteArray;
}

QList<QString> &ImagePlayerPrivate::getPathList(const DeviceWatcherType type)
{
    switch (type) {
    case DWT_SDDisk: {
        return m_SDPathList;
    }
    case DWT_USBDisk:
    default: {
        return m_USBPathList;
    }
    }
}

void ImagePlayerPrivate::exitImagePlayer()
{
}
