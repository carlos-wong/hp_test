#include "SDDiskWidget.h"
#include "UserInterfaceUtility.h"
#include "BusinessLogic/BusinessLogicUtility.h"
#include "AutoConnect.h"
#include "MessageBox.h"
#include "UserInterface/MainWidget/DiskWidget/MusicWidget/MusicWidget.h"
#include "UserInterface/MainWidget/DiskWidget/ImageWidget/ImageWidget.h"
#include "UserInterface/MainWidget/DiskWidget/VideoWidget/VideoWidget.h"
#include "SDDiskListViewWidget/SDDiskListViewWidget.h"
#include "SDDiskToolWidget/SDDiskToolWidget.h"
#include "UserInterfaceUtility.h"
#include "BmpWidget.h"
#include "BusinessLogic/Setting.h"
#include "BusinessLogic/Bluetooth.h"
#include <QPainter>
#include <QDomDocument>
#include <QVariant>

namespace SourceString {
static const QString No_SD_Device(QObject::tr("No SD Device..."));
static const QString Unsupport(QObject::tr("Unsupport..."));
static const QString Scanning_SD_Device(QObject::tr("Scanning SD Device..."));
static const QString Remove_SD_Device(QObject::tr("Remove SD Device..."));
}

class SDDiskWidgetPrivate
{
    Q_DISABLE_COPY(SDDiskWidgetPrivate)
public:
    explicit SDDiskWidgetPrivate(SDDiskWidget* parent);
    ~SDDiskWidgetPrivate();
    void initialize();
    void initializeParent();
    void initializeListView();
    void initializeToolWidget();
    void initializeMessageBox();
    void initializeTimer();
    void connectAllSlots();
    SDDiskListViewWidget* m_SDDiskListViewWidget;
    SDDiskToolWidget* m_SDDiskToolWidget;
    MessageBox* m_SDDiskDeviceMessageBox;
    Widget::Type m_WidgetType;
    MultimediaType m_MultimediaType;
    DeviceWatcherStatus m_DeviceWatcherStatus;
    QTimer* m_Timer;
    BmpWidget* m_Background;
private:
    SDDiskWidget* m_Parent;
};

SDDiskWidget::SDDiskWidget(QWidget *parent)
    : QWidget(parent)
    , m_Private(new SDDiskWidgetPrivate(this))
{
}

SDDiskWidget::~SDDiskWidget()
{
}

void SDDiskWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(0, 42, 800, 438, this);
}

void SDDiskWidget::onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status)
{
    switch (destinationType) {
    case Widget::T_Home:
    case Widget::T_Back: {
        if (m_Private->m_SDDiskDeviceMessageBox->isVisible()) {
            if (WidgetStatus::RequestShow == status) {
                g_Widget->setWidgetType(Widget::T_ViewPaper0, Widget::T_SDDisk, WidgetStatus::RequestShow);
            }
        }
        break;
    }
    case Widget::T_SDDisk: {
        if (WidgetStatus::RequestShow == status) {
            qDebug() << __PRETTY_FUNCTION__ << __LINE__;
#ifdef DESKTOP_AMD64
            g_Widget->setWidgetType(Widget::T_SDDiskMusic, Widget::T_ViewPaper0, WidgetStatus::RequestShow);
#else
            if (Widget::T_SDDisk != m_Private->m_WidgetType) {
                qDebug() << __PRETTY_FUNCTION__ << __LINE__;
                g_Widget->setWidgetType(m_Private->m_WidgetType, Widget::T_SDDisk, WidgetStatus::RequestShow);
            } else {
                qDebug() << __PRETTY_FUNCTION__ << __LINE__;
                if (MT_Music == m_Private->m_MultimediaType) {
                    qDebug() << __PRETTY_FUNCTION__ << __LINE__;
                    g_Widget->setWidgetType(Widget::T_SDDiskMusic, Widget::T_ViewPaper0, WidgetStatus::RequestShow);
                } else if (MT_Image == m_Private->m_MultimediaType) {
                    qDebug() << __PRETTY_FUNCTION__ << __LINE__;
                    g_Widget->setWidgetType(Widget::T_SDDiskImage, Widget::T_ViewPaper0, WidgetStatus::RequestShow);
                } else if (MT_Video == m_Private->m_MultimediaType) {
                    qDebug() << __PRETTY_FUNCTION__ << __LINE__;
                    g_Widget->setWidgetType(Widget::T_SDDiskVideo, Widget::T_ViewPaper0, WidgetStatus::RequestShow);
                } else {
                    qDebug() << __PRETTY_FUNCTION__ << __LINE__;
                    g_Widget->setWidgetType(Widget::T_SDDisk, Widget::T_ViewPaper0, WidgetStatus::Show);
                    if (DWS_Remove == m_Private->m_DeviceWatcherStatus) {
                        m_Private->m_DeviceWatcherStatus = DWS_Empty;
                    }
                }
            }
#endif
        } else if (WidgetStatus::Show == status) {
            if ((DWS_Empty == m_Private->m_DeviceWatcherStatus)
                    || (DWS_Unsupport == m_Private->m_DeviceWatcherStatus)
                    || (DWS_Remove == m_Private->m_DeviceWatcherStatus)) {
                m_Private->initializeTimer();
                m_Private->m_Timer->start();
            }
            if (DWS_Remove == m_Private->m_DeviceWatcherStatus) {
                m_Private->m_SDDiskDeviceMessageBox->setText(SourceString::Remove_SD_Device);
            } else if (DWS_Empty == m_Private->m_DeviceWatcherStatus) {
                m_Private->m_SDDiskDeviceMessageBox->setText(SourceString::No_SD_Device);
            }
            if (!isVisible()) {
                setVisible(true);
            }
        }
        break;
    }
    case Widget::T_SDMusic:
    case Widget::T_USBMusic:
    case Widget::T_SDImage:
    case Widget::T_USBImage:
    case Widget::T_SDImageFullScreen:
    case Widget::T_USBImageFullScreen:
    case Widget::T_SDVideo:
    case Widget::T_SDVideoFullScreen: {
        if (WidgetStatus::Show == status) {
            if ((destinationType == Widget::T_SDMusic)
                    && (requestType == Widget::T_SDDiskMusic)) {
                m_Private->m_MultimediaType = MT_Music;
                m_Private->m_WidgetType = Widget::T_SDMusic;
            } else if ((destinationType == Widget::T_SDImage)
                       && (requestType == Widget::T_SDDiskImage)) {
                m_Private->m_MultimediaType = MT_Image;
                m_Private->m_WidgetType = Widget::T_SDImage;
            } else if ((destinationType == Widget::T_SDVideo)
                       && (requestType == Widget::T_SDDiskVideo)) {
                m_Private->m_MultimediaType = MT_Video;
                m_Private->m_WidgetType = Widget::T_SDVideo;
            }
            if (Widget::T_SDDisk != m_Private->m_WidgetType) {
                m_Private->m_SDDiskDeviceMessageBox->setVisible(false);
            }
            setVisible(false);
        }
        break;
    }
    case Widget::T_SDDiskMusic: {
        if (WidgetStatus::Show == status) {
            if (MT_Idle == m_Private->m_MultimediaType) {
                m_Private->m_WidgetType = Widget::T_SDDiskMusic;
            }
            m_Private->m_SDDiskDeviceMessageBox->setVisible(false);
            setVisible(true);
        }
        break;
    }
    case Widget::T_SDDiskImage: {
        if (WidgetStatus::Show == status) {
            if (MT_Idle == m_Private->m_MultimediaType) {
                m_Private->m_WidgetType = Widget::T_SDDiskImage;
            }
            if (m_Private->m_SDDiskDeviceMessageBox->isVisible()) {
                m_Private->m_SDDiskDeviceMessageBox->setVisible(false);
            }
            setVisible(true);
        }
        break;
    }
    case Widget::T_SDDiskVideo: {
        if (WidgetStatus::Show == status) {
            if (MT_Idle == m_Private->m_MultimediaType) {
                m_Private->m_WidgetType = Widget::T_SDDiskVideo;
            }
            if (m_Private->m_SDDiskDeviceMessageBox->isVisible()) {
                m_Private->m_SDDiskDeviceMessageBox->setVisible(false);
            }
            setVisible(true);
        }
        break;
    }
    default: {
        if (WidgetStatus::Show == status) {
            if (isVisible()) {
                if (DWS_Remove == m_Private->m_DeviceWatcherStatus) {
                    m_Private->m_DeviceWatcherStatus = DWS_Empty;
                }
                setVisible(false);
            }
        }
        break;
    }
    }
}

void SDDiskWidget::onDeviceWatcherStatus(const DeviceWatcherType type, const DeviceWatcherStatus status)
{
    qDebug() << "SDDiskWidget::onDeviceWatcherStatus" << type << status;
    if (DWT_SDDisk == type) {
        m_Private->m_DeviceWatcherStatus = status;
        switch (status) {
        case DWS_Empty: {
            m_Private->m_SDDiskDeviceMessageBox->setText(SourceString::No_SD_Device);
            m_Private->m_SDDiskDeviceMessageBox->setAutoHide(false);
            m_Private->m_SDDiskDeviceMessageBox->setVisible(true);
            break;
        }
        case DWS_Unsupport: {
            if (DST_Off == g_Setting->getDisplayScreenType()) {
                g_Setting->setDisplayScreen(DST_On);
            }
            m_Private->m_SDDiskDeviceMessageBox->setText(SourceString::Unsupport);
            m_Private->m_SDDiskDeviceMessageBox->setAutoHide(false);
            m_Private->m_SDDiskDeviceMessageBox->setVisible(true);
            g_Widget->setWidgetType(Widget::T_SDDisk, Widget::T_SDDisk, WidgetStatus::RequestShow);
            break;
        }
        case DWS_Busy: {
            if (DST_Off == g_Setting->getDisplayScreenType()) {
                g_Setting->setDisplayScreen(DST_On);
            }
            if (NULL != m_Private->m_Timer) {
                m_Private->m_Timer->stop();
            }
            m_Private->m_SDDiskDeviceMessageBox->setText(SourceString::Scanning_SD_Device);
            m_Private->m_SDDiskDeviceMessageBox->setAutoHide(false);
            m_Private->m_SDDiskDeviceMessageBox->setVisible(true);
            g_Widget->setWidgetType(Widget::T_SDDisk, Widget::T_SDDisk, WidgetStatus::RequestShow);
            break;
        }
        case DWS_Ready: {
            if (NULL != m_Private->m_Timer) {
                m_Private->m_Timer->stop();
            }
            if (isVisible()) {
                if (Widget::T_SDDisk != m_Private->m_WidgetType) {
                    g_Widget->setWidgetType(m_Private->m_WidgetType, Widget::T_SDDisk, WidgetStatus::RequestShow);
                }
            }
            break;
        }
        case DWS_Remove: {
            m_Private->m_WidgetType = Widget::T_SDDisk;
            m_Private->m_MultimediaType = MT_Idle;
            m_Private->m_SDDiskDeviceMessageBox->setText(SourceString::Remove_SD_Device);
            m_Private->m_SDDiskDeviceMessageBox->setAutoHide(false);
            m_Private->m_SDDiskDeviceMessageBox->setVisible(true);
            if (isVisible()) {
                g_Widget->setWidgetType(Widget::T_SDDisk, Widget::T_SDDisk, WidgetStatus::Show);
            }
            break;
        }
        default: {
            break;
        }
        }
    }
}

void SDDiskWidget::onMusicPlayerFileNames(const DeviceWatcherType type, const QString &xml)
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
            Widget::Type type = Widget::T_Undefine;
            while (!node.isNull()) {
                if (node.isElement()) {
                    QDomElement element = node.toElement();
                    if (!element.isNull()) {
                        if (QString("SDPersistant") == element.tagName()) {
                            QDomElement node = element.toElement();
                            if (node.isElement()) {
                                if (!node.toElement().isNull()) {
                                    if (node.isElement()) {
                                        if (!node.toElement().text().isEmpty()) {
                                            type = Widget::T_SDDiskMusic;
                                        }
                                    }
                                }
                            }
                        } else if (QString("SDType") == element.tagName()) {
                            QDomElement node = element.toElement();
                            if (node.isElement()) {
                                if (!node.toElement().isNull()) {
                                    if (node.isElement()) {
                                        if (!node.toElement().text().isEmpty()) {
                                            if (MT_Music == node.toElement().text().toInt()) {
                                                if (Widget::T_SDDiskMusic == type) {
                                                    type = Widget::T_SDMusic;
                                                    break;
                                                } else {
                                                    type = Widget::T_SDDiskMusic;
                                                    break;
                                                }
                                            } else if (MT_Idle == node.toElement().text().toInt()) {
                                                type = Widget::T_SDDiskMusic;
                                                break;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                node = node.nextSibling();
            }
            if (Widget::T_Undefine != type) {
                m_Private->m_WidgetType = type;
            }
        }
    }
}

void SDDiskWidget::onMusicPlayerID3TagChange(const DeviceWatcherType type, const int index, const QString &fileName, const QString &title, const QString &artist, const QString &album, const int endTime)
{
    if (DWT_SDDisk == type) {
        m_Private->m_MultimediaType = MT_Music;
        m_Private->m_WidgetType = Widget::T_SDMusic;
    }
}

void SDDiskWidget::onImagePlayerFileNames(const DeviceWatcherType type, const QString &xml)
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
            Widget::Type type = Widget::T_Undefine;
            while (!node.isNull()) {
                if (node.isElement()) {
                    QDomElement element = node.toElement();
                    if (!element.isNull()) {
                        if (QString("SDPersistant") == element.tagName()) {
                            QDomElement node = element.toElement();
                            if (node.isElement()) {
                                if (!node.toElement().isNull()) {
                                    if (node.isElement()) {
                                        if (!node.toElement().text().isEmpty()) {
                                            type = Widget::T_SDDiskImage;
                                        }
                                    }
                                }
                            }
                        } else if (QString("SDType") == element.tagName()) {
                            QDomElement node = element.toElement();
                            if (node.isElement()) {
                                if (!node.toElement().isNull()) {
                                    if (node.isElement()) {
                                        if (!node.toElement().text().isEmpty()) {
                                            if (MT_Image == node.toElement().text().toInt()) {
                                                if (Widget::T_SDDiskImage == type) {
                                                    type = Widget::T_SDImage;
                                                    break;
                                                } else {
                                                    type = Widget::T_SDDiskImage;
                                                    break;
                                                }
                                            } else {
                                                type = Widget::T_Undefine;
                                                break;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                node = node.nextSibling();
            }
            if (Widget::T_Undefine != type) {
                m_Private->m_WidgetType = type;
            }
        }
    }
}

void SDDiskWidget::onImagePlayerChange(const DeviceWatcherType type, const QString &filePath, const int index, const int percent, const int rotate)
{
    if (DWT_SDDisk == type) {
        m_Private->m_MultimediaType = MT_Image;
        m_Private->m_WidgetType = Widget::T_SDImage;
    }
}

void SDDiskWidget::onVideoPlayerFileNames(const DeviceWatcherType type, const QString &xml)
{
    if (DWT_SDDisk == type) {
        //        qDebug() << "SDDiskVideoListViewWidget::onVideoPlayerFileNames" << type << xml;
        QDomDocument document;
        document.setContent(xml);
        QDomElement root = document.documentElement();
        if ((!root.isNull())
                && (root.isElement())
                && (QString("VideoPlayer") == root.toElement().tagName())
                && (root.hasChildNodes())) {
            QDomNode node = root.firstChild();
            Widget::Type type = Widget::T_Undefine;
            while (!node.isNull()) {
                if (node.isElement()) {
                    QDomElement element = node.toElement();
                    if (!element.isNull()) {
                        if (QString("SDPersistant") == element.tagName()) {
                            QDomElement node = element.toElement();
                            if (node.isElement()) {
                                if (!node.toElement().isNull()) {
                                    if (node.isElement()) {
                                        if (!node.toElement().text().isEmpty()) {
                                            type = Widget::T_SDDiskVideo;
                                        }
                                    }
                                }
                            }
                        } else if (QString("SDType") == element.tagName()) {
                            QDomElement node = element.toElement();
                            if (node.isElement()) {
                                if (!node.toElement().isNull()) {
                                    if (node.isElement()) {
                                        if (!node.toElement().text().isEmpty()) {
                                            if (MT_Video == node.toElement().text().toInt()) {
                                                if (Widget::T_SDDiskVideo == type) {
                                                    type = Widget::T_SDVideo;
                                                    break;
                                                } else {
                                                    type = Widget::T_SDDiskVideo;
                                                    break;
                                                }
                                            } else {
                                                type = Widget::T_Undefine;
                                                break;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                node = node.nextSibling();
            }
            if (Widget::T_Undefine != type) {
                m_Private->m_WidgetType = type;
            }
        }
    }
}

void SDDiskWidget::onVideoPlayerInformation(const int type, const int index, const QString &fileName, const int endTime)
{
    if (DWT_SDDisk == type) {
        m_Private->m_MultimediaType = MT_Video;
        m_Private->m_WidgetType = Widget::T_SDVideo;
    }
}

void SDDiskWidget::onTimeout()
{
    if (isVisible()) {
        g_Widget->setWidgetType(Widget::T_ViewPaper0, Widget::T_SDDisk, WidgetStatus::RequestShow);
    }
}

SDDiskWidgetPrivate::SDDiskWidgetPrivate(SDDiskWidget *parent)
    : m_Parent(parent)
{
    m_SDDiskListViewWidget = NULL;
    m_SDDiskToolWidget = NULL;
    m_SDDiskDeviceMessageBox = NULL;
    m_WidgetType = Widget::T_SDDisk;
    m_MultimediaType = MT_Idle;
    m_DeviceWatcherStatus = DWS_Empty;
    m_Timer = NULL;
    initializeParent();
    initialize();
    connectAllSlots();
}

SDDiskWidgetPrivate::~SDDiskWidgetPrivate()
{
}

void SDDiskWidgetPrivate::initialize()
{
    m_Background = new BmpWidget(m_Parent);
    m_Background->setBackgroundBmpPath(QString(":/Images/hw_01_main_background.png"));
    g_Widget->geometryFit(0, 0, 800, 438, m_Background);
    m_SDDiskListViewWidget = new SDDiskListViewWidget(m_Parent);
    m_SDDiskToolWidget = new SDDiskToolWidget(m_Parent);
    m_SDDiskDeviceMessageBox = new MessageBox(m_Parent);
    m_SDDiskDeviceMessageBox->setText(SourceString::No_SD_Device);
    m_SDDiskDeviceMessageBox->setAutoHide(false);
    m_SDDiskDeviceMessageBox->setFontPointSize(15 * g_Widget->widthScalabilityFactor());
    m_SDDiskDeviceMessageBox->setBackgroundBmpPath(QString(":/Images/MessageBoxBackground.png"));
    g_Widget->geometryFit(0, 0, 800, 438, m_SDDiskDeviceMessageBox);
    m_SDDiskDeviceMessageBox->setVisible(true);
}

void SDDiskWidgetPrivate::initializeParent()
{
    m_Parent->setVisible(false);
}

void SDDiskWidgetPrivate::initializeListView()
{
    if (NULL == m_SDDiskListViewWidget) {
        m_SDDiskToolWidget = new SDDiskToolWidget(m_Parent);
    }
}

void SDDiskWidgetPrivate::initializeToolWidget()
{
    if (NULL == m_SDDiskToolWidget) {
        m_SDDiskListViewWidget = new SDDiskListViewWidget(m_Parent);
    }
}

void SDDiskWidgetPrivate::initializeMessageBox()
{
    if (NULL == m_SDDiskDeviceMessageBox) {
        m_SDDiskDeviceMessageBox = new MessageBox(m_Parent);
    }
}

void SDDiskWidgetPrivate::initializeTimer()
{
    if (NULL == m_Timer) {
        m_Timer = new QTimer(m_Parent);
        m_Timer->setSingleShot(true);
        m_Timer->setInterval(3000);
        Qt::ConnectionType type = static_cast<Qt::ConnectionType>(Qt::AutoConnection | Qt::UniqueConnection);
        QObject::connect(m_Timer,  ARKSENDER(timeout()),
                         m_Parent, ARKRECEIVER(onTimeout()),
                         type);
    }
}

void SDDiskWidgetPrivate::connectAllSlots()
{
    connectSignalAndSlotByNamesake(g_Widget, m_Parent, ARKRECEIVER(onWidgetTypeChange(const Widget::Type, const Widget::Type, const QString &)));
    connectSignalAndSlotByNamesake(g_Multimedia, m_Parent, ARKRECEIVER(onDeviceWatcherStatus(const int, const int)));
    connectSignalAndSlotByNamesake(g_Multimedia, m_Parent, ARKRECEIVER(onMusicPlayerFileNames(const int, const QString &)));
    connectSignalAndSlotByNamesake(g_Multimedia, m_Parent, ARKRECEIVER(onMusicPlayerID3TagChange(const int, const int, const QString &, const QString&, const QString&, const QString&, const int)));
    connectSignalAndSlotByNamesake(g_Multimedia, m_Parent, ARKRECEIVER(onImagePlayerFileNames(const int, const QString &)));
    connectSignalAndSlotByNamesake(g_Multimedia, m_Parent, ARKRECEIVER(onImagePlayerChange(const int, const QString &, const int, const int, const int)));
    connectSignalAndSlotByNamesake(g_Multimedia, m_Parent, ARKRECEIVER(onVideoPlayerFileNames(const int, const QString&)));
    connectSignalAndSlotByNamesake(g_Multimedia, m_Parent, ARKRECEIVER(onVideoPlayerInformation(const int, const int, const QString , const int)));
}
