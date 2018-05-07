#include "USBDiskWidget.h"
#include "UserInterfaceUtility.h"
#include "BusinessLogic/BusinessLogicUtility.h"
#include "AutoConnect.h"
#include "MessageBox.h"
#include "USBDiskListViewWidget/USBDiskListViewWidget.h"
#include "USBDiskToolWidget/USBDiskToolWidget.h"
#include "BmpWidget.h"
#include "UserInterfaceUtility.h"
#include "BusinessLogic/Setting.h"
#include "BusinessLogic/Bluetooth.h"
#include <QPainter>
#include <QDomDocument>
#include <QVariant>

namespace SourceString {
static const QString No_USB_Device(QObject::tr("No USB Device..."));
static const QString Unsupport(QObject::tr("Unsupport..."));
static const QString Scanning_USB_Device(QObject::tr("Scanning USB Device..."));
static const QString Remove_USB_Device(QObject::tr("Remove USB Device..."));
}

class USBDiskWidgetPrivate
{
    Q_DISABLE_COPY(USBDiskWidgetPrivate)
public:
    explicit USBDiskWidgetPrivate(USBDiskWidget* parent);
    ~USBDiskWidgetPrivate();
    void initialize();
    void initializeTimer();
    void connectAllSlots();
    USBDiskListViewWidget* m_USBDiskListViewWidget;
    USBDiskToolWidget* m_USBDiskToolWidget;
    MessageBox* m_USBDiskDeviceMessageBox;
    Widget::Type m_WidgetType;
    MultimediaType m_MultimediaType;
    DeviceWatcherStatus m_DeviceWatcherStatus;
    QTimer* m_Timer;
private:
    USBDiskWidget* m_Parent;
};

USBDiskWidget::USBDiskWidget(QWidget *parent)
    : QWidget(parent)
    , m_Private(new USBDiskWidgetPrivate(this))
{   
}

USBDiskWidget::~USBDiskWidget()
{
}

void USBDiskWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(0, 52, 800, 428, this);
}

void USBDiskWidget::onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status)
{
    switch (destinationType) {
    case Widget::T_Home:
    case Widget::T_Back: {
        if (isVisible()) {
            if (m_Private->m_USBDiskDeviceMessageBox->isVisible()) {
                if (WidgetStatus::RequestShow == status) {
                    g_Widget->setWidgetType(Widget::T_ViewPaper0, Widget::T_USBDisk, WidgetStatus::RequestShow);
                }
            }
        }
        break;
    }
    case Widget::T_USBDisk: {
        if (WidgetStatus::RequestShow == status) {
            if (Widget::T_USBDisk != m_Private->m_WidgetType) {
                g_Widget->setWidgetType(m_Private->m_WidgetType, Widget::T_USBDisk, WidgetStatus::RequestShow);
            } else {
                if (MT_Music == m_Private->m_MultimediaType) {
                    g_Widget->setWidgetType(Widget::T_USBDiskMusic, Widget::T_ViewPaper0, WidgetStatus::RequestShow);
                } else if (MT_Image == m_Private->m_MultimediaType) {
                    g_Widget->setWidgetType(Widget::T_USBDiskImage, Widget::T_ViewPaper0, WidgetStatus::RequestShow);
                } else if (MT_Video == m_Private->m_MultimediaType) {
                    g_Widget->setWidgetType(Widget::T_USBDiskVideo, Widget::T_ViewPaper0, WidgetStatus::RequestShow);
                } else {
                    g_Widget->setWidgetType(Widget::T_USBDisk, Widget::T_ViewPaper0, WidgetStatus::Show);
                    if (DWS_Remove == m_Private->m_DeviceWatcherStatus) {
                        m_Private->m_DeviceWatcherStatus = DWS_Empty;
                    }
                }
            }
        } else if (WidgetStatus::Show == status) {
            if ((DWS_Empty == m_Private->m_DeviceWatcherStatus)
                    || (DWS_Unsupport == m_Private->m_DeviceWatcherStatus)
                    || (DWS_Remove == m_Private->m_DeviceWatcherStatus)) {
                m_Private->initializeTimer();
                m_Private->m_Timer->start();
            }
            if (DWS_Remove == m_Private->m_DeviceWatcherStatus) {
                m_Private->m_USBDiskDeviceMessageBox->setText(SourceString::Remove_USB_Device);
            } else if (DWS_Empty == m_Private->m_DeviceWatcherStatus) {
                m_Private->m_USBDiskDeviceMessageBox->setText(SourceString::No_USB_Device);
            }
            setVisible(true);
        }
        break;
    }
    case Widget::T_USBDiskMusic: {
        if (WidgetStatus::Show == status) {
            if (MT_Idle == m_Private->m_MultimediaType) {
                m_Private->m_WidgetType = Widget::T_USBDiskMusic;
            }
            m_Private->m_USBDiskDeviceMessageBox->setVisible(false);
            if (!isVisible()) {
                setVisible(true);
            }
        }
        break;
    }
    case Widget::T_USBDiskImage: {
        if (WidgetStatus::Show == status) {
            if (MT_Idle == m_Private->m_MultimediaType) {
                m_Private->m_WidgetType = Widget::T_USBDiskImage;
            }
            m_Private->m_USBDiskDeviceMessageBox->setVisible(false);
            setVisible(true);
        }
        break;
    }
    case Widget::T_USBDiskVideo: {
        if (WidgetStatus::Show == status) {
            if (MT_Idle == m_Private->m_MultimediaType) {
                m_Private->m_WidgetType = Widget::T_USBDiskVideo;
            }
            m_Private->m_USBDiskDeviceMessageBox->setVisible(false);
            setVisible(true);
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
    case Widget::T_SDVideoFullScreen:
    case Widget::T_USBVideo:
    case Widget::T_USBVideoFullScreen: {
        if (WidgetStatus::Show == status) {
            if (Widget::T_USBDisk != m_Private->m_WidgetType) {
                m_Private->m_USBDiskDeviceMessageBox->setVisible(false);
            }
            setVisible(false);
        }
        break;
    }
    default: {
        if (WidgetStatus::Show == status) {
            if (isVisible()) {
                if (NULL != m_Private->m_Timer) {
                    m_Private->m_Timer->stop();
                }
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

void USBDiskWidget::onDeviceWatcherStatus(const DeviceWatcherType type, const DeviceWatcherStatus status)
{
    if (DWT_USBDisk == type) {
        m_Private->m_DeviceWatcherStatus = status;
        switch (status) {
        case DWS_Empty: {
            m_Private->m_USBDiskDeviceMessageBox->setText(SourceString::No_USB_Device);
            m_Private->m_USBDiskDeviceMessageBox->setAutoHide(false);
            m_Private->m_USBDiskDeviceMessageBox->setVisible(true);
            break;
        }
        case DWS_Unsupport: {
            if (DST_Off == g_Setting->getDisplayScreenType()) {
                g_Setting->setDisplayScreen(DST_On);
            }
            m_Private->m_USBDiskDeviceMessageBox->setText(SourceString::Unsupport);
            m_Private->m_USBDiskDeviceMessageBox->setAutoHide(false);
            m_Private->m_USBDiskDeviceMessageBox->setVisible(true);
            g_Widget->setWidgetType(Widget::T_USBDisk, Widget::T_USBDisk, WidgetStatus::RequestShow);
            break;
        }
        case DWS_Busy: {
            if (DST_Off == g_Setting->getDisplayScreenType()) {
                g_Setting->setDisplayScreen(DST_On);
            }
            if (NULL != m_Private->m_Timer) {
                m_Private->m_Timer->stop();
            }
            m_Private->m_USBDiskDeviceMessageBox->setText(SourceString::Scanning_USB_Device);
            m_Private->m_USBDiskDeviceMessageBox->setAutoHide(false);
            m_Private->m_USBDiskDeviceMessageBox->setVisible(true);
            g_Widget->setWidgetType(Widget::T_USBDisk, Widget::T_USBDisk, WidgetStatus::RequestShow);
            break;
        }
        case DWS_Ready: {
            qDebug() << __PRETTY_FUNCTION__ << __LINE__ << m_Private->m_WidgetType;
            if (NULL != m_Private->m_Timer) {
                m_Private->m_Timer->stop();
            }
            if (isVisible()) {
                qDebug() << __PRETTY_FUNCTION__ << __LINE__ << m_Private->m_WidgetType;
                if (Widget::T_USBDisk != m_Private->m_WidgetType) {
                    g_Widget->setWidgetType(m_Private->m_WidgetType, Widget::T_USBDisk, WidgetStatus::RequestShow);
                }
            }
            break;
        }
        case DWS_Remove: {
            m_Private->m_WidgetType = Widget::T_USBDisk;
            m_Private->m_MultimediaType = MT_Idle;
            m_Private->m_USBDiskDeviceMessageBox->setText(SourceString::Remove_USB_Device);
            m_Private->m_USBDiskDeviceMessageBox->setAutoHide(false);
            m_Private->m_USBDiskDeviceMessageBox->setVisible(true);
            if (isVisible()) {
                g_Widget->setWidgetType(Widget::T_USBDisk, Widget::T_USBDisk, WidgetStatus::Show);
            }
            break;
        }
        default: {
            break;
        }
        }
    }
}

void USBDiskWidget::onMusicPlayerFileNames(const DeviceWatcherType type, const QString &xml)
{
    if (DWT_USBDisk == type) {
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
                        if (QString("USBPersistant") == element.tagName()) {
                            QDomElement node = element.toElement();
                            if (node.isElement()) {
                                if (!node.toElement().isNull()) {
                                    if (node.isElement()) {
                                        if (!node.toElement().text().isEmpty()) {
                                            type = Widget::T_USBDiskMusic;
                                        }
                                    }
                                }
                            }
                        } else if (QString("USBType") == element.tagName()) {
                            QDomElement node = element.toElement();
                            if (node.isElement()) {
                                if (!node.toElement().isNull()) {
                                    if (node.isElement()) {
                                        if (!node.toElement().text().isEmpty()) {
                                            if (MT_Music == node.toElement().text().toInt()) {
                                                if (Widget::T_USBDiskMusic == type) {
                                                    type = Widget::T_USBMusic;
                                                    break;
                                                }/* else {
                                                    type = Widget::T_USBDiskMusic;
                                                    break;
                                                }*/
                                            } else if (MT_Idle == node.toElement().text().toInt()) {
                                                type = Widget::T_USBDiskMusic;
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

void USBDiskWidget::onMusicPlayerID3TagChange(const int type, const int index, const QString &fileName, const QString &title, const QString &artist, const QString &album, const int endTime)
{
    if (DWT_USBDisk == type) {
        m_Private->m_MultimediaType = MT_Music;
        m_Private->m_WidgetType = Widget::T_USBMusic;
    }
}

void USBDiskWidget::onImagePlayerFileNames(const DeviceWatcherType type, const QString &xml)
{
    if (DWT_USBDisk == type) {
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
                        if (QString("USBPersistant") == element.tagName()) {
                            QDomElement node = element.toElement();
                            if (node.isElement()) {
                                if (!node.toElement().isNull()) {
                                    if (node.isElement()) {
                                        if (!node.toElement().text().isEmpty()) {
                                            type = Widget::T_USBDiskImage;
                                        }
                                    }
                                }
                            }
                        } else if (QString("USBType") == element.tagName()) {
                            QDomElement node = element.toElement();
                            if (node.isElement()) {
                                if (!node.toElement().isNull()) {
                                    if (node.isElement()) {
                                        if (!node.toElement().text().isEmpty()) {
                                            if (MT_Image == node.toElement().text().toInt()) {
                                                if (Widget::T_USBDiskImage == type) {
                                                    type = Widget::T_USBImage;
                                                    break;
                                                } else {
                                                    type = Widget::T_USBDiskImage;
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

void USBDiskWidget::onImagePlayerChange(const int type, const QString &filePath, const int index, const int percent, const int rotate)
{
    if (DWT_USBDisk == type) {
        m_Private->m_MultimediaType = MT_Image;
        m_Private->m_WidgetType = Widget::T_USBImage;
    }
}

void USBDiskWidget::onVideoPlayerFileNames(const DeviceWatcherType type, const QString &xml)
{
    if (DWT_USBDisk == type) {
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
                        if (QString("USBPersistant") == element.tagName()) {
                            QDomElement node = element.toElement();
                            if (node.isElement()) {
                                if (!node.toElement().isNull()) {
                                    if (node.isElement()) {
                                        if (!node.toElement().text().isEmpty()) {
                                            type = Widget::T_USBDiskVideo;
                                        }
                                    }
                                }
                            }
                        } else if (QString("USBType") == element.tagName()) {
                            QDomElement node = element.toElement();
                            if (node.isElement()) {
                                if (!node.toElement().isNull()) {
                                    if (node.isElement()) {
                                        if (!node.toElement().text().isEmpty()) {
                                            if (MT_Video == node.toElement().text().toInt()) {
                                                if (Widget::T_USBDiskVideo == type) {
                                                    type = Widget::T_USBVideo;
                                                    break;
                                                } else {
                                                    type = Widget::T_USBDiskVideo;
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

void USBDiskWidget::onVideoPlayerInformation(const DeviceWatcherType type, const int index, const QString &fileName, const int endTime)
{
    if (DWT_USBDisk == type) {
        m_Private->m_MultimediaType = MT_Video;
        m_Private->m_WidgetType = Widget::T_USBVideo;
    }
}

void USBDiskWidget::onTimeout()
{
    if (isVisible()) {
        g_Widget->setWidgetType(Widget::T_ViewPaper0, Widget::T_USBDisk, WidgetStatus::RequestShow);
    }
}

USBDiskWidgetPrivate::USBDiskWidgetPrivate(USBDiskWidget *parent)
    : m_Parent(parent)
{
    m_USBDiskListViewWidget = NULL;
    m_USBDiskToolWidget = NULL;
    m_USBDiskDeviceMessageBox = NULL;
    m_WidgetType = Widget::T_USBDisk;
    m_MultimediaType = MT_Idle;
    m_DeviceWatcherStatus = DWS_Empty;
    m_Timer = NULL;
    initialize();
    connectAllSlots();
}

USBDiskWidgetPrivate::~USBDiskWidgetPrivate()
{
}

void USBDiskWidgetPrivate::initialize()
{
    m_Parent->setVisible(false);
    m_USBDiskListViewWidget = new USBDiskListViewWidget(m_Parent);
    m_USBDiskToolWidget = new USBDiskToolWidget(m_Parent);
    m_USBDiskDeviceMessageBox = new MessageBox(m_Parent);
    m_USBDiskDeviceMessageBox->setText(SourceString::No_USB_Device);
    m_USBDiskDeviceMessageBox->setAutoHide(false);
    m_USBDiskDeviceMessageBox->setFontPointSize(15 * g_Widget->widthScalabilityFactor());
    m_USBDiskDeviceMessageBox->setBackgroundBmpPath(QString(":/Images/MessageBoxBackground.png"));
    g_Widget->geometryFit(0, 0, 800, 428, m_USBDiskDeviceMessageBox);
    m_USBDiskDeviceMessageBox->setVisible(true);
}

void USBDiskWidgetPrivate::initializeTimer()
{
    if (NULL == m_Timer) {
        m_Timer = new QTimer(m_Parent);
        m_Timer->setSingleShot(true);
        m_Timer->setInterval(2500);
        QObject::connect(m_Timer,  ARKSENDER(timeout()),
                         m_Parent, ARKRECEIVER(onTimeout()));
    }
}

void USBDiskWidgetPrivate::connectAllSlots()
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
