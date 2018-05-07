#include "MusicWidget.h"
#include "BusinessLogic/Setting.h"
#include "UserInterfaceUtility.h"
#include "MessageBox.h"
#include "MusicInformationWidget/MusicInformationWidget.h"
#include "MusicSeekBarWidget/MusicSeekBarWidget.h"
#include "MusicToolWidget/MusicToolWidget.h"
#include "AutoConnect.h"
#include "UserInterfaceUtility.h"
#include <QDomDocument>

namespace SourceString {
static const QString Unsupport(QObject::tr("Unsupport..."));
}

class MusicWidgetPrivate
{
    Q_DISABLE_COPY(MusicWidgetPrivate)
public:
    explicit MusicWidgetPrivate(MusicWidget *parent);
    ~MusicWidgetPrivate();
    void initializeParent();
    void initializeMusicInformationWidget();
    void initializeMusicSeekBarWidget();
    void initializeMusicToolWidget();
    void initializeMessageBox();
    void connectAllSlots();
    Widget::Type m_Type;
    MusicInformationWidget* m_MusicInformationWidget;
    MusicSeekBarWidget* m_MusicSeekBarWidget;
    MusicToolWidget* m_MusicToolWidget;
    MessageBox* m_MessageBox;
    DeviceWatcherType m_DeviceWatcherType;
    int m_SDDiskLastIndex;
    int m_SDDiskElapsed;
    int m_USBDiskLastIndex;
    int m_USBDiskElapsed;
    bool m_RequestShow;
    bool m_HardKeyHandler;
    MusicPlayerPlayStatus m_MusicPlayerPlayStatus;
private:
    MusicWidget* m_Parent;
};

MusicWidget::MusicWidget(QWidget *parent)
    : QWidget(parent)
    , m_Private(new MusicWidgetPrivate(this))
{
}

MusicWidget::~MusicWidget()
{
}

void MusicWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(0, 0, 800, 480, this);
}

void MusicWidget::onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status)
{
    switch (destinationType) {
    case Widget::T_Back: {
        if (isVisible()) {
            if (WidgetStatus::RequestShow == status) {
                if (DWT_SDDisk == m_Private->m_DeviceWatcherType) {
                    g_Widget->setWidgetType(Widget::T_SDDiskMusic, Widget::T_SDMusic, WidgetStatus::RequestShow);
                } else if (DWT_USBDisk == m_Private->m_DeviceWatcherType) {
                    g_Widget->setWidgetType(Widget::T_USBDiskMusic, Widget::T_SDMusic, WidgetStatus::RequestShow);
                }
            }
        }
        break;
    }
    case Widget::T_Home: {
        if (isVisible()) {
            if (WidgetStatus::RequestShow == status) {
                g_Widget->setWidgetType(Widget::T_ViewPaper0, Widget::T_Undefine, WidgetStatus::RequestShow);
            }
        }
        break;
    }
    case Widget::T_SDMusic: {
        if (WidgetStatus::RequestShow == status) {
            if ((Widget::T_SDDisk == requestType)
                    && (-1 != m_Private->m_SDDiskLastIndex)
                    && (DWT_SDDisk != m_Private->m_DeviceWatcherType)) {
                g_Multimedia->musicPlayerPlayListViewIndex(DWT_SDDisk, m_Private->m_SDDiskLastIndex, m_Private->m_SDDiskElapsed);
                m_Private->m_DeviceWatcherType = DWT_SDDisk;
                g_Widget->setWidgetType(Widget::T_SDMusic, Widget::T_Undefine, WidgetStatus::Show);
            } else {
                if (m_Private->m_DeviceWatcherType == DWT_Undefine) {
                    if (Widget::T_SDDiskMusic != requestType) {
                        if (Widget::T_SDMusic == m_Private->m_Type) {
                            g_Widget->setWidgetType(Widget::T_SDDisk, requestType, WidgetStatus::Show);
                        }
                        m_Private->m_Type = Widget::T_Undefine;
                    } else {
                        g_Widget->setWidgetType(Widget::T_SDMusic, Widget::T_Undefine, WidgetStatus::Show);
                    }
                } else {
                    g_Widget->setWidgetType(m_Private->m_Type, requestType, WidgetStatus::Show);
                }
            }
        } else if (WidgetStatus::Show == status) {
            g_Multimedia->musicInitialize();
            m_Private->initializeMusicToolWidget();
            m_Private->initializeMusicSeekBarWidget();
            m_Private->initializeMusicInformationWidget();
            m_Private->m_Type = Widget::T_SDMusic;
            setVisible(true);
        }
        break;
    }
    case Widget::T_USBMusic: {
        if (WidgetStatus::RequestShow == status) {
            qDebug() << __PRETTY_FUNCTION__ << __LINE__ << requestType << m_Private->m_USBDiskLastIndex << m_Private->m_DeviceWatcherType;
            if ((Widget::T_USBDisk == requestType)
                    && (-1 != m_Private->m_USBDiskLastIndex)
                    && (DWT_USBDisk != m_Private->m_DeviceWatcherType)) {
                qDebug() << __PRETTY_FUNCTION__ << __LINE__;
                g_Multimedia->musicPlayerPlayListViewIndex(DWT_USBDisk, m_Private->m_USBDiskLastIndex, m_Private->m_USBDiskElapsed);
                m_Private->m_DeviceWatcherType = DWT_USBDisk;
                g_Widget->setWidgetType(Widget::T_USBMusic, Widget::T_Undefine, WidgetStatus::Show);
            } else {
                qDebug() << __PRETTY_FUNCTION__ << __LINE__;
                if (m_Private->m_DeviceWatcherType == DWT_Undefine) {
                    qDebug() << __PRETTY_FUNCTION__ << __LINE__;
                    if (Widget::T_USBDiskMusic != requestType) {
                        if (Widget::T_USBMusic == m_Private->m_Type) {
                            qDebug() << __PRETTY_FUNCTION__ << __LINE__;
                            g_Widget->setWidgetType(Widget::T_USBDisk, requestType, WidgetStatus::Show);
                        }
                        m_Private->m_Type = Widget::T_Undefine;
                    } else {
                        g_Widget->setWidgetType(Widget::T_USBMusic, Widget::T_Undefine, WidgetStatus::Show);
                        m_Private->m_Type = Widget::T_USBMusic;
                    }
                } else {
                    qDebug() << __PRETTY_FUNCTION__ << __LINE__;
                    g_Widget->setWidgetType(m_Private->m_Type, requestType, WidgetStatus::Show);
                }
            }
        } else if (WidgetStatus::Show == status) {
            qDebug() << __PRETTY_FUNCTION__ << __LINE__;
            g_Multimedia->musicInitialize();
            m_Private->initializeMusicToolWidget();
            m_Private->initializeMusicSeekBarWidget();
            m_Private->initializeMusicInformationWidget();
            m_Private->m_Type = Widget::T_USBMusic;
            setVisible(true);
        }
        break;
    }
    default : {
        if (WidgetStatus::Show == status) {
            if (isVisible()) {
                setVisible(false);
            }
        }
        break;
    }
    }
}

void MusicWidget::onDeviceWatcherStatus(const DeviceWatcherType type, const DeviceWatcherStatus status)
{
    qDebug() << __PRETTY_FUNCTION__ << m_Private->m_DeviceWatcherType << type;
    if ((m_Private->m_DeviceWatcherType == type)
            && (DWS_Remove == status)) {
        if (isVisible()) {
            //            g_Widget->setWidgetType(Widget::T_Radio, Widget::T_Music, WidgetStatus::RequestShow);
            if (DWT_SDDisk == m_Private->m_DeviceWatcherType) {
                g_Widget->setWidgetType(Widget::T_SDDisk, Widget::T_SDMusic, WidgetStatus::RequestShow);
            } else if (DWT_USBDisk == m_Private->m_DeviceWatcherType) {
                g_Widget->setWidgetType(Widget::T_USBDisk, Widget::T_USBMusic, WidgetStatus::RequestShow);
            }
        }
        m_Private->m_DeviceWatcherType = DWT_Undefine;
    }
}

void MusicWidget::onMusicPlayerFileNames(const DeviceWatcherType type, const QString &xml)
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
                        if (QString("SDPersistant") == element.tagName()) {
                            QDomElement node = element.toElement();
                            if (node.isElement()) {
                                if (!node.toElement().isNull()) {
                                    if (node.isElement()) {
                                        if (!node.toElement().text().isEmpty()) {
                                            m_Private->m_SDDiskLastIndex = node.toElement().text().split(QChar('-')).at(0).toInt();
                                            m_Private->m_SDDiskElapsed = node.toElement().text().split(QChar('-')).at(1).toInt();
                                        }
                                    }
                                }
                            }
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
                        if (QString("USBPersistant") == element.tagName()) {
                            QDomElement node = element.toElement();
                            if (node.isElement()) {
                                if (!node.toElement().isNull()) {
                                    if (node.isElement()) {
                                        if (!node.toElement().text().isEmpty()) {
                                            m_Private->m_USBDiskLastIndex = node.toElement().text().split(QChar('-')).at(0).toInt();
                                            m_Private->m_USBDiskElapsed = node.toElement().text().split(QChar('-')).at(1).toInt();
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                node = node.nextSibling();
            }
        }
    }
}

void MusicWidget::onMusicPlayerID3TagChange(const DeviceWatcherType type, const int index, const QString &fileName, const QString& title, const QString& artist, const QString& album, const int endTime)
{    
    m_Private->m_DeviceWatcherType = type;
    qDebug() << __PRETTY_FUNCTION__ << m_Private->m_DeviceWatcherType << type;
    if (DWT_SDDisk == type) {
        m_Private->m_SDDiskLastIndex = index;
    } else if (DWT_USBDisk == type) {
        m_Private->m_USBDiskLastIndex = index;
    }
    //    if (!isVisible()
    //            && m_Private->m_RequestShow) {
    //        m_Private->m_RequestShow = false;
    //        g_Widget->setWidgetType(Widget::T_Music, Widget::T_Undefine, WidgetStatus::Show);
    //    }
}

void MusicWidget::onMusicPlayerElapsedInformation(const int elapsedTime, const int elapsedMillesimal)
{
    if (DWT_SDDisk == m_Private->m_DeviceWatcherType) {
        m_Private->m_SDDiskElapsed = elapsedTime;
    } else if (DWT_USBDisk == m_Private->m_DeviceWatcherType) {
        m_Private->m_USBDiskElapsed = elapsedTime;
    }
}

void MusicWidget::onMusicPlayerPlayStatus(const DeviceWatcherType type, const MusicPlayerPlayStatus status)
{
    m_Private->m_MusicPlayerPlayStatus = status;
    switch (status) {
    case MPPS_Unsupport: {
        m_Private->initializeMessageBox();
        m_Private->m_MessageBox->setVisible(true);
        break;
    }
    case MPPS_Start: {
        if (NULL != m_Private->m_MessageBox) {
            m_Private->m_MessageBox->setVisible(false);
        }
        break;
    }
    case MPPS_Play: {
        break;
    }
    case MPPS_Pause: {
        break;
    }
    case MPPS_Stop: {
        break;
    }
    case MPPS_Exit: {
        m_Private->m_DeviceWatcherType = DWT_Undefine;
        break;
    }
    default : {
        break;
    }
    }
}

void MusicWidget::onArkProtocol(const ArkProtocolWrapper &protocol)
{
    switch (protocol.type) {
    case AT_Key: {
        struct ArkKey* key = arkProtocolWrapperToArkKey(protocol);
        if ((NULL != key)
                && (!protocol.handler)
                && (Multimedia::F_ImageUnfocus == g_Multimedia->getImageFocus())
                && (m_Private->m_HardKeyHandler)) {
            if ((AKS_Release == key->status)
                    || (AKS_Active == key->status)) {
                switch (key->type) {
                case AKT_CommonPrevious: {
                    g_Multimedia->musicPlayerPlayPreviousListViewIndex();
                    break;
                }
                case AKT_CommonNext: {
                    g_Multimedia->musicPlayerPlayNextListViewIndex();
                    break;
                }
                default: {
                    break;
                }
                }
            }
        }
        break;
    }
    default: {
        break;
    }
    }
}

void MusicWidget::onHolderChange(const AudioSource oldHolder, const AudioSource newHolder)
{
    m_Private->m_HardKeyHandler = (AS_Music == newHolder);
}

MusicWidgetPrivate::MusicWidgetPrivate(MusicWidget *parent)
    : m_Parent(parent)
{
    m_Type = Widget::T_Undefine;
    m_MusicInformationWidget = NULL;
    m_MusicSeekBarWidget = NULL;
    m_MusicToolWidget = NULL;
    m_MessageBox = NULL;
    m_DeviceWatcherType = DWT_Undefine;
    m_SDDiskLastIndex = -1;
    m_SDDiskElapsed = 0;
    m_USBDiskLastIndex = -1;
    m_USBDiskElapsed = 0;
    m_RequestShow = false;
    m_HardKeyHandler = false;
    m_MusicPlayerPlayStatus = MPPS_Exit;
    initializeParent();
    connectAllSlots();
}

MusicWidgetPrivate::~MusicWidgetPrivate()
{
}

void MusicWidgetPrivate::initializeParent()
{
    m_Parent->setVisible(false);
}

void MusicWidgetPrivate::initializeMusicInformationWidget()
{
    if (NULL == m_MusicInformationWidget) {
        m_MusicInformationWidget = new MusicInformationWidget(m_Parent);
    }
}

void MusicWidgetPrivate::initializeMusicSeekBarWidget()
{
    if (NULL == m_MusicSeekBarWidget) {
        m_MusicSeekBarWidget = new MusicSeekBarWidget(m_Parent);
    }
}

void MusicWidgetPrivate::initializeMusicToolWidget()
{
    if (NULL == m_MusicToolWidget) {
        m_MusicToolWidget = new MusicToolWidget(m_Parent);
    }
}

void MusicWidgetPrivate::initializeMessageBox()
{
    if (NULL == m_MessageBox) {
        m_MessageBox = new MessageBox(m_Parent);
        m_MessageBox->setFontPointSize(15 * g_Widget->widthScalabilityFactor());
        m_MessageBox->setBackgroundBmpPath(QString(":/Images/MessageBoxBackground.png"));
        m_MessageBox->setText(SourceString::Unsupport);
        g_Widget->geometryFit(0, 52, 800, 348, m_MessageBox);
        m_MessageBox->setAutoHide(false);
    }
}

void MusicWidgetPrivate::connectAllSlots()
{
    connectSignalAndSlotByNamesake(g_Setting, m_Parent, ARKRECEIVER(onArkProtocol(const ArkProtocolWrapper&)));
    connectSignalAndSlotByNamesake(g_Widget, m_Parent, ARKRECEIVER(onWidgetTypeChange(const Widget::Type, const Widget::Type, const QString &)));
    connectSignalAndSlotByNamesake(g_Multimedia, m_Parent, ARKRECEIVER(onDeviceWatcherStatus(const int, const int)));
    connectSignalAndSlotByNamesake(g_Multimedia, m_Parent, ARKRECEIVER(onMusicPlayerFileNames(const int, const QString &)));
    connectSignalAndSlotByNamesake(g_Multimedia, m_Parent, ARKRECEIVER(onMusicPlayerID3TagChange(const int, const int, const QString &, const QString&, const QString&, const QString&, const int)));
    connectSignalAndSlotByNamesake(g_Multimedia, m_Parent, ARKRECEIVER(onMusicPlayerElapsedInformation(const int, const int)));
    connectSignalAndSlotByNamesake(g_Multimedia, m_Parent, ARKRECEIVER(onMusicPlayerPlayStatus(const int, const int)));
    connectSignalAndSlotByNamesake(g_Audio, m_Parent, ARKRECEIVER(onHolderChange(const int, const int)));
}
