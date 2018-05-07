#include "VideoWidget.h"
#include "BusinessLogic/BusinessLogicUtility.h"
#include "UserInterfaceUtility.h"
#include "VideoSeekBarWidget/VideoSeekBarWidget.h"
#include "MessageBox.h"
#include "UserInterfaceUtility.h"
#include "TextWidget.h"
#include "BusinessLogic/Setting.h"
#include "ark_api.h"
#include <fcntl.h>
#include <QPainter>
#include <QBrush>
#include <QLabel>
#include <QFileInfo>
#include <QTimer>
#include <QMouseEvent>
#include <QDomDocument>

namespace SourceString {
static const QString Unsupport(QObject::tr("Unsupport..."));
static const QString PutDownHandBreak(QObject::tr("Please don't watch video while driving!"));
}

class VideoWidgetPrivate
{
    Q_DISABLE_COPY(VideoWidgetPrivate)
public:
    explicit VideoWidgetPrivate(VideoWidget * parent);
    ~VideoWidgetPrivate();
    void initializeTimer();
    void initializeParent();
    void initializeTextWidget();
    void initializeSeekBarWidget();
    void initializeToolBarWidget();
    void initializeMessageBox();
    void connectAllSlots();
    void handleHandBreakNotReady();
    void handleHandBreakReady();
    QString convertTime(const int time);
    TextWidget* m_TextWidget;
    VideoSeekBarWidget* m_VideoSeekBarWidget;
    VideoToolBarWidget* m_VideoToolBarWidget;
    QTimer* m_Timer;
    QRect m_TouchRect;
    MessageBox* m_MessageBox;
    Widget::Type m_Type;
    DeviceWatcherType m_DeviceWatcherType;
    int m_SDDiskLastIndex;
    int m_SDDiskElapsed;
    int m_USBDiskLastIndex;
    int m_USBDiskElapsed;
    bool m_RequestShow;
    bool m_HardKeyHandler;
    bool m_IsHandBreak;
private:
    VideoWidget* m_Parent;
};

VideoWidget::VideoWidget(QWidget *parent)
    : QWidget(parent)
    , m_Private(new VideoWidgetPrivate(this))
{
}

VideoWidget::~VideoWidget()
{
}

void VideoWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(0, 0, 800, 480, this);
}

void VideoWidget::mousePressEvent(QMouseEvent *event)
{
    m_Private->m_Timer->stop();
}

void VideoWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (isVisible()) {
        if (m_Private->m_TouchRect.contains(event->pos())) {
            if (m_Private->m_TouchRect == g_Widget->geometryFit(0, 0, 800, 480)) {
                if (Widget::T_SDVideo == m_Private->m_Type) {
                    g_Widget->setWidgetType(Widget::T_SDVideo, Widget::T_SDVideoFullScreen, WidgetStatus::Show);
                } else if (Widget::T_USBVideo == m_Private->m_Type) {
                    g_Widget->setWidgetType(Widget::T_USBVideo, Widget::T_USBVideoFullScreen, WidgetStatus::Show);
                }
            } else {
                if (event->pos().y() < 332 * g_Widget->heightScalabilityFactor()) {
                    if (Widget::T_SDVideo == m_Private->m_Type) {
                        g_Widget->setWidgetType(Widget::T_SDVideoFullScreen, Widget::T_SDVideo, WidgetStatus::Show);
                    } else if (Widget::T_USBVideo == m_Private->m_Type) {
                        g_Widget->setWidgetType(Widget::T_USBVideoFullScreen, Widget::T_USBVideo, WidgetStatus::Show);
                    }
                } else {
                    if (Widget::T_SDVideo == m_Private->m_Type) {
                        g_Widget->setWidgetType(Widget::T_SDVideo, Widget::T_SDVideo, WidgetStatus::Show);
                    } else if (Widget::T_USBVideo == m_Private->m_Type) {
                        g_Widget->setWidgetType(Widget::T_USBVideo, Widget::T_USBVideo, WidgetStatus::Show);
                    }
                }
            }
        }
    }
}

void VideoWidget::onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status)
{
    switch (destinationType) {
    case Widget::T_Back: {
        if (isVisible()) {
            if (WidgetStatus::RequestShow == status) {
                g_Widget->setWidgetType(Widget::T_ViewPaper0, Widget::T_SDVideo, WidgetStatus::RequestShow);
                if (DWT_SDDisk == m_Private->m_DeviceWatcherType) {
                    g_Widget->setWidgetType(Widget::T_SDDiskVideo, Widget::T_SDVideo, WidgetStatus::RequestShow);
                } else if (DWT_USBDisk == m_Private->m_DeviceWatcherType) {
                    g_Widget->setWidgetType(Widget::T_USBDiskVideo, Widget::T_USBVideo, WidgetStatus::RequestShow);
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
    case Widget::T_SDVideo: {
        if (WidgetStatus::RequestShow == status) {
            qDebug() << __PRETTY_FUNCTION__ << __LINE__ << m_Private->m_SDDiskLastIndex << m_Private->m_DeviceWatcherType << m_Private->m_USBDiskLastIndex;
            qDebug() << __PRETTY_FUNCTION__ << __LINE__ << requestType << m_Private->m_DeviceWatcherType;
            if ((Widget::T_SDDisk == requestType)
                    && (-1 != m_Private->m_SDDiskLastIndex)) {
                qDebug() << __PRETTY_FUNCTION__ << __LINE__;
                if (DWT_SDDisk != m_Private->m_DeviceWatcherType) {
                    qDebug() << __PRETTY_FUNCTION__ << __LINE__;
                    g_Setting->setLauncherTVOUT(false);
                    if (ACS_ReversingOff == g_Setting->getReversing()) {
                        if (QFile::exists(QString("/tmp/video"))) {
                            QFile::remove(QString("/tmp/video"));
                        }
                    }
                    g_Multimedia->videoPlayerPlayListViewIndex(DWT_SDDisk, m_Private->m_SDDiskLastIndex, 0, 0, 800 * g_Widget->widthScalabilityFactor(), 480 * g_Widget->heightScalabilityFactor(), m_Private->m_SDDiskElapsed);
                    m_Private->m_DeviceWatcherType = DWT_SDDisk;
                    g_Widget->setWidgetType(Widget::T_SDVideo, Widget::T_Undefine, WidgetStatus::Show);
                } else {
                    qDebug() << __PRETTY_FUNCTION__ << __LINE__;
                    g_Widget->setWidgetType(Widget::T_SDVideo, Widget::T_Undefine, WidgetStatus::Show);
                }
                qDebug() << __PRETTY_FUNCTION__ << __LINE__;
            } else {
                qDebug() << __PRETTY_FUNCTION__ << __LINE__;
                if (m_Private->m_DeviceWatcherType == DWT_Undefine) {
                    qDebug() << __PRETTY_FUNCTION__ << __LINE__;
                    if (Widget::T_SDDiskVideo != requestType) {
                        qDebug() << __PRETTY_FUNCTION__ << __LINE__;
                        qDebug() << "m_Private->m_Type" << m_Private->m_Type;
                        Widget::Type tmp = m_Private->m_Type;
                        m_Private->m_Type = Widget::T_Undefine;
                        if (Widget::T_SDVideo == tmp) {
                            qDebug() << __PRETTY_FUNCTION__ << __LINE__;
                            g_Widget->setWidgetType(Widget::T_SDDisk, requestType, WidgetStatus::RequestShow);
                        }
                        qDebug() << __PRETTY_FUNCTION__ << __LINE__;
                    } else {
                        qDebug() << __PRETTY_FUNCTION__ << __LINE__;
                        g_Widget->setWidgetType(Widget::T_SDVideo, Widget::T_Undefine, WidgetStatus::Show);
                    }
                    qDebug() << __PRETTY_FUNCTION__ << __LINE__;
                } else {
                    qDebug() << __PRETTY_FUNCTION__ << __LINE__;
                    g_Widget->setWidgetType(m_Private->m_Type, requestType, WidgetStatus::Show);
                }
                qDebug() << __PRETTY_FUNCTION__ << __LINE__;
            }
        } else if (WidgetStatus::Show == status) {
            qDebug() << __PRETTY_FUNCTION__ << __LINE__;
            g_Setting->setLauncherTVOUT(false);
            if (ACS_ReversingOff == g_Setting->getReversing()) {
                if (QFile::exists(QString("/tmp/video"))) {
                    QFile::remove(QString("/tmp/video"));
                }
            }
            m_Private->m_TouchRect = g_Widget->geometryFit(0, 52, 800, 428);
            m_Private->initializeTimer();
            m_Private->m_Timer->start();
            m_Private->initializeTextWidget();
            m_Private->m_TextWidget->setVisible(true);
            qDebug() << __PRETTY_FUNCTION__ << __LINE__ << isVisible();
            if (!isVisible()) {
                g_Multimedia->videoInitialize();
                g_Multimedia->videoPlayerSetPlayStatus(VPPS_Play);
            }
            connectSignalAndSlotByNamesake(g_Setting, this, ARKRECEIVER(onArkProtocol(const ArkProtocolWrapper&)));
            m_Private->m_Type = Widget::T_SDVideo;
            setVisible(true);
        }
        break;
    }
    case Widget::T_USBVideo: {
        if (WidgetStatus::RequestShow == status) {
            qDebug() << __PRETTY_FUNCTION__ << __LINE__ << m_Private->m_SDDiskLastIndex << m_Private->m_DeviceWatcherType << m_Private->m_USBDiskLastIndex;
#ifdef DESKTOP_AMD64
            g_Widget->setWidgetType(Widget::T_USBVideo, Widget::T_Undefine, WidgetStatus::Show);
#else
            if ((Widget::T_USBDisk == requestType)
                    && (-1 != m_Private->m_USBDiskLastIndex)) {
                qDebug() << __PRETTY_FUNCTION__ << __LINE__;
                if (DWT_USBDisk != m_Private->m_DeviceWatcherType) {
                    qDebug() << __PRETTY_FUNCTION__ << __LINE__;
                    g_Setting->setLauncherTVOUT(false);
                    if (ACS_ReversingOff == g_Setting->getReversing()) {
                        if (QFile::exists(QString("/tmp/video"))) {
                            QFile::remove(QString("/tmp/video"));
                        }
                    }
                    g_Multimedia->videoPlayerPlayListViewIndex(DWT_USBDisk, m_Private->m_USBDiskLastIndex, 0, 0, 800 * g_Widget->widthScalabilityFactor(), 480 * g_Widget->heightScalabilityFactor(), m_Private->m_USBDiskElapsed);
                    m_Private->m_DeviceWatcherType = DWT_USBDisk;
                    g_Widget->setWidgetType(Widget::T_USBVideo, Widget::T_Undefine, WidgetStatus::Show);
                    //                    m_Private->m_NeedSuspendToggle = true;
                } else {
                    qDebug() << __PRETTY_FUNCTION__ << __LINE__;
                    //g_Multimedia->videoPlayerSetPlayStatus(VPPS_SuspendToggle);
                    g_Widget->setWidgetType(Widget::T_USBVideo, Widget::T_Undefine, WidgetStatus::Show);
                }
            } else {
                qDebug() << __PRETTY_FUNCTION__ << __LINE__;
                if (m_Private->m_DeviceWatcherType == DWT_Undefine) {
                    qDebug() << __PRETTY_FUNCTION__ << __LINE__;
                    if (Widget::T_USBDiskVideo != requestType) {
                        qDebug() << __PRETTY_FUNCTION__ << __LINE__ << m_Private->m_Type;
                        if (Widget::T_USBVideo == m_Private->m_Type) {
                            qDebug() << __PRETTY_FUNCTION__ << __LINE__;
                            g_Widget->setWidgetType(Widget::T_USBDisk, requestType, WidgetStatus::Show);
                        }
                        m_Private->m_Type = Widget::T_Undefine;
                    } else {
                        qDebug() << __PRETTY_FUNCTION__ << __LINE__;
                        g_Widget->setWidgetType(Widget::T_USBVideo, Widget::T_Undefine, WidgetStatus::Show);
                    }
                } else {
                    qDebug() << __PRETTY_FUNCTION__ << __LINE__;
                    g_Widget->setWidgetType(m_Private->m_Type, requestType, WidgetStatus::Show);
                }
            }
#endif
        } else if (WidgetStatus::Show == status) {
            qDebug() << __PRETTY_FUNCTION__ << __LINE__;
            g_Setting->setLauncherTVOUT(false);
            if (ACS_ReversingOff == g_Setting->getReversing()) {
                if (QFile::exists(QString("/tmp/video"))) {
                    QFile::remove(QString("/tmp/video"));
                }
            }
            m_Private->m_TouchRect = g_Widget->geometryFit(0, 52, 800, 428);
            m_Private->initializeTimer();
            m_Private->m_Timer->start();
            m_Private->initializeTextWidget();
            m_Private->m_TextWidget->setVisible(true);
            qDebug() << __PRETTY_FUNCTION__ << __LINE__ << isVisible();
            if (!isVisible()) {
                g_Multimedia->videoInitialize();
                g_Multimedia->videoPlayerSetPlayStatus(VPPS_Play);
            }
            connectSignalAndSlotByNamesake(g_Setting, this, ARKRECEIVER(onArkProtocol(const ArkProtocolWrapper&)));
            m_Private->m_Type = Widget::T_USBVideo;
            setVisible(true);
        }
        break;
    }
    case Widget::T_SDVideoFullScreen:
    case Widget::T_USBVideoFullScreen: {
        if (WidgetStatus::RequestShow == status) {
            if (isVisible()) {
                g_Widget->setWidgetType(destinationType, requestType, WidgetStatus::Show);
            } else {
                if (Widget::T_SDVideoFullScreen == destinationType) {
                    g_Widget->setWidgetType(Widget::T_SDVideo, requestType, WidgetStatus::RequestShow);
                } else if (Widget::T_USBVideoFullScreen == destinationType) {
                    g_Widget->setWidgetType(Widget::T_USBVideo, requestType, WidgetStatus::RequestShow);
                }
            }
        } else if (WidgetStatus::Show == status) {
            qDebug() << __PRETTY_FUNCTION__ << __LINE__ << isVisible();
            g_Setting->setLauncherTVOUT(false);
            if (ACS_ReversingOff == g_Setting->getReversing()) {
                if (QFile::exists(QString("/tmp/video"))) {
                    QFile::remove(QString("/tmp/video"));
                }
            }
            if (!isVisible()) {
                g_Multimedia->videoInitialize();
                g_Multimedia->videoPlayerSetPlayStatus(VPPS_Play);
            }
            m_Private->m_TouchRect = g_Widget->geometryFit(0, 0, 800, 480);
            if (NULL != m_Private->m_TextWidget) {
                m_Private->m_TextWidget->setVisible(false);
            }
            if (NULL != m_Private->m_Timer) {
                m_Private->m_Timer->stop();
            }
            setVisible(true);
        }
        break;
    }
    default : {
        if (status == WidgetStatus::Show) {
            if (isVisible()) {
                g_Multimedia->videoPlayerSetPlayStatus(VPPS_Pause);
                setVisible(false);
                g_Setting->setLauncherTVOUT(true);
                if (NULL != m_Private->m_Timer) {
                    m_Private->m_Timer->stop();
                }
            }
        }
        break;
    }
    }
}

void VideoWidget::onVideoPlayerFileNames(const DeviceWatcherType type, const QString &xml)
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
                && (QString("VideoPlayer") == root.toElement().tagName())
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

void VideoWidget::onVideoPlayerPlayStatus(const DeviceWatcherType type, const VideoPlayerPlayStatus playStatus)
{
    switch (playStatus) {
    case VPPS_Start: {
        if (m_Private->m_IsHandBreak) {
            m_Private->handleHandBreakNotReady();
        }
        else {
            m_Private->m_MessageBox->setVisible(false);
        }
        //        m_Private->m_NeedSuspendToggle = true;
        break;
    }
    case VPPS_Play: {
        qDebug()<< "=======VPPS_Play======>isVisible(): "<<isVisible();
        if (isVisible()) {
            if  (!m_Private->m_Timer->isActive()) {
                m_Private->m_Timer->start();
            }

            //视频检测开关是否打开
            bool  isOpen = SettingPersistent::getVideoWarningStatus();

            //手刹是否放下
            bool isBreak = SettingPersistent::getCurrentBrakeStatus();

            if (isOpen && !isBreak) {  //手刹没有放下,弹框提示
                m_Private->m_IsHandBreak = true;
                m_Private->handleHandBreakNotReady();
            }
            else {
                m_Private->m_IsHandBreak = false;
                m_Private->m_MessageBox->setVisible(false);
                m_Private->m_VideoToolBarWidget->setEnabled(true);
            }
        }
        //        m_Private->m_NeedSuspendToggle = true;
        break;
    }
    case VPPS_Pause: {
        m_Private->m_Timer->stop();
        //        m_Private->m_NeedSuspendToggle = true;
        break;
    }
    case VPPS_Exit: {
        m_Private->m_Timer->stop();
        m_Private->m_DeviceWatcherType = DWT_Undefine;
        //        m_Private->m_NeedSuspendToggle = false;
        break;
    }
    case VPPS_Unsupport: {
        m_Private->m_MessageBox->setText(SourceString::Unsupport);
        m_Private->m_MessageBox->setAutoHide(false);
        m_Private->m_MessageBox->setVisible(true);
        //        m_Private->m_NeedSuspendToggle = false;
        break;
    }
    default : {
        break;
    }
    }
}

void VideoWidget::onDeviceWatcherStatus(const DeviceWatcherType type, const DeviceWatcherStatus status)
{
    qDebug() << __PRETTY_FUNCTION__ << __LINE__ << type << status;
    qDebug() << __PRETTY_FUNCTION__ << __LINE__ << m_Private->m_DeviceWatcherType << m_Private->m_DeviceWatcherType;
    if ((m_Private->m_DeviceWatcherType == type)
            && (DWS_Remove == status)) {
        if (isVisible()) {
            if (DWT_SDDisk == m_Private->m_DeviceWatcherType) {
                g_Widget->setWidgetType(Widget::T_SDDisk, Widget::T_SDVideo, WidgetStatus::RequestShow);
            } else if (DWT_USBDisk == m_Private->m_DeviceWatcherType) {
                g_Widget->setWidgetType(Widget::T_USBDisk, Widget::T_USBVideo, WidgetStatus::RequestShow);
            }
        }
        m_Private->m_DeviceWatcherType = DWT_Undefine;
        //        m_Private->m_NeedSuspendToggle = false;
    }
    static bool flag = true;
    if (flag) {
        flag = false;
        m_Private->initializeTextWidget();
        m_Private->initializeTimer();
        m_Private->initializeSeekBarWidget();
        m_Private->initializeToolBarWidget();
        m_Private->initializeMessageBox();
        connectSignalAndSlotByNamesake(g_Audio, this, ARKRECEIVER(onHolderChange(const int, const int)));
        connectSignalAndSlotByNamesake(g_Multimedia, this, ARKRECEIVER(onVideoPlayerFileNames(const int, const QString&)));
        connectSignalAndSlotByNamesake(g_Multimedia, this, ARKRECEIVER(onVideoPlayerPlayStatus(const int, const int)));
        connectSignalAndSlotByNamesake(g_Multimedia, this, ARKRECEIVER(onVideoPlayerInformation(const int, const int, const QString &, const int)));
        connectSignalAndSlotByNamesake(g_Multimedia, this, ARKRECEIVER(onVideoPlayerElapsedInformation(const int, const int)));
    }
}

void VideoWidget::onVideoPlayerInformation(const DeviceWatcherType type, const int index, const QString &fileName, const int endTime)
{
    m_Private->m_TextWidget->setText(fileName);
    m_Private->m_DeviceWatcherType = type;
    if (DWT_SDDisk == m_Private->m_DeviceWatcherType) {
        m_Private->m_SDDiskLastIndex = index;
    } else if (DWT_USBDisk == m_Private->m_DeviceWatcherType) {
        m_Private->m_USBDiskLastIndex = index;
    }
    m_Private->m_Timer->start();
}

void VideoWidget::onVideoPlayerElapsedInformation(const int elapsedTime, const int elapsedMillesimal)
{
    if (DWT_SDDisk == m_Private->m_DeviceWatcherType) {
        m_Private->m_SDDiskElapsed = elapsedTime;
    } else if (DWT_USBDisk == m_Private->m_DeviceWatcherType) {
        m_Private->m_USBDiskElapsed = elapsedTime;
    }
}

void VideoWidget::onArkProtocol(const ArkProtocolWrapper &protocol)
{
    switch (protocol.type) {
    case AT_Key: {
        struct ArkKey* key = arkProtocolWrapperToArkKey(protocol);
        if ((NULL != key)
                && (!protocol.handler)
                && (isVisible())
                && (m_Private->m_HardKeyHandler)) {
            if ((AKS_Release == key->status)
                    || (AKS_Active == key->status)) {
                switch (key->type) {
                case AKT_CommonPrevious: {
                    g_Multimedia->videoPlayerPlayPreviousListViewIndex();
                    break;
                }
                case AKT_CommonNext: {
                    g_Multimedia->videoPlayerPlayNextListViewIndex();
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

    case AT_Car:{
        struct ArkCar* car = arkProtocolWrapperToArkCar(protocol);
        qWarning() << __PRETTY_FUNCTION__ << __LINE__ << car->type << car->status;
        switch(car->type)
        case ACT_Brake:{
            switch(car->status){
            case ACS_BrakeOn:  //打开手刹
                SettingPersistent::setCurrentBrakeStatus(true);
                if  (isVisible() && m_Private->m_IsHandBreak) {
                    m_Private->m_IsHandBreak = false;
                    m_Private->handleHandBreakReady();
                }
                break;
            case ACS_BrakeOff: //关闭手刹
                SettingPersistent::setCurrentBrakeStatus(false);
                //视频警告检测开关是否打开
                bool isOpen = SettingPersistent::getVideoWarningStatus();
                if (isVisible() && isOpen)
                {
                    m_Private->handleHandBreakNotReady();
                    m_Private->m_IsHandBreak = true;
                }
                break;
            }
        }
            break;
        }

    default: {
        break;
    }
    }
}

void VideoWidget::onHolderChange(const int oldHolder, const int newHolder)
{
    m_Private->m_HardKeyHandler = (AS_Video == newHolder);
}

void VideoWidget::onTimeout()
{
    if (Widget::T_SDVideo == m_Private->m_Type) {
        g_Widget->setWidgetType(Widget::T_SDVideoFullScreen, Widget::T_SDVideo, WidgetStatus::Show);
    } else if (Widget::T_USBVideo == m_Private->m_Type) {
        g_Widget->setWidgetType(Widget::T_USBVideoFullScreen, Widget::T_USBVideo, WidgetStatus::Show);
    }
}

VideoWidgetPrivate::VideoWidgetPrivate(VideoWidget * parent)
    : m_Parent(parent)
{
    m_TextWidget = NULL;
    m_VideoSeekBarWidget = NULL;
    m_VideoToolBarWidget = NULL;
    m_Timer = NULL;
    m_TouchRect = QRect(0, 0, 0, 0);
    m_MessageBox = NULL;
    m_Type = Widget::T_Undefine;
    m_DeviceWatcherType = DWT_Undefine;
    m_SDDiskLastIndex = -1;
    m_SDDiskElapsed = 0;
    m_USBDiskLastIndex = -1;
    m_USBDiskElapsed = 0;
    m_RequestShow = false;
    m_HardKeyHandler = false;
    m_IsHandBreak = false;
    initializeParent();
    connectAllSlots();
}

VideoWidgetPrivate::~VideoWidgetPrivate()
{
}

void VideoWidgetPrivate::initializeTimer()
{
    if (NULL == m_Timer) {
        m_Timer = new QTimer(m_Parent);
        m_Timer->setInterval(5000);
        m_Timer->setSingleShot(true);
        QObject::connect(m_Timer,  ARKSENDER(timeout()),
                         m_Parent, ARKRECEIVER(onTimeout()));
    }
}

void VideoWidgetPrivate::initializeParent()
{
    m_Parent->setVisible(false);
}

void VideoWidgetPrivate::initializeTextWidget()
{
    if (NULL == m_TextWidget) {
        m_TextWidget = new TextWidget(m_Parent);
        m_TextWidget->setLanguageType(TextWidget::T_NoTranslate);
        m_TextWidget->setAlignmentFlag(Qt::AlignLeft | Qt::AlignVCenter);
        g_Widget->geometryFit(0, 52, 800, 30, m_TextWidget);
        m_TextWidget->setVisible(true);
    }
}

void VideoWidgetPrivate::initializeSeekBarWidget()
{
    if (NULL == m_VideoSeekBarWidget) {
        m_VideoSeekBarWidget = new VideoSeekBarWidget(m_Parent);
    }
}

void VideoWidgetPrivate::initializeToolBarWidget()
{
    if (NULL == m_VideoToolBarWidget) {
        m_VideoToolBarWidget = new VideoToolBarWidget(m_Parent);
    }
}

void VideoWidgetPrivate::initializeMessageBox()
{
    if (NULL == m_MessageBox) {
        m_MessageBox = new MessageBox(m_Parent);
        g_Widget->geometryFit(0, 52, 800, 348, m_MessageBox);
        m_MessageBox->setVisible(false);
        m_MessageBox->setBackgroundBmpPath(QString(":/Images/MessageBoxBackground.png"));
        m_MessageBox->setFontPointSize(15 * g_Widget->widthScalabilityFactor());
    }
}

void VideoWidgetPrivate::connectAllSlots()
{
    connectSignalAndSlotByNamesake(g_Widget, m_Parent, ARKRECEIVER(onWidgetTypeChange(const Widget::Type, const Widget::Type, const QString &)));
    connectSignalAndSlotByNamesake(g_Multimedia, m_Parent, ARKRECEIVER(onDeviceWatcherStatus(const int, const int)));
}

QString VideoWidgetPrivate::convertTime(const int time)
{
    QString hour("%1");
    QString minute("%1");
    QString second("%1");
    return hour.arg((time / 60) / 60, 2, 10, QChar('0'))
            + QString(":") + minute.arg((time / 60) % 60, 2, 10, QChar('0'))
            + QString(":") + second.arg(time % 60, 2, 10, QChar('0'));
}

void VideoWidgetPrivate::handleHandBreakNotReady()
{
    g_Multimedia->videoPlayerSetPlayStatus(VPPS_Pause);
    m_VideoToolBarWidget->setEnabled(false);
    m_MessageBox->setText(SourceString::PutDownHandBreak);
    m_MessageBox->setAutoHide(false);
    m_MessageBox->setVisible(true);
}

void VideoWidgetPrivate::handleHandBreakReady()
{
    m_MessageBox->setVisible(false);
    m_VideoToolBarWidget->setEnabled(true);
    g_Multimedia->videoPlayerSetPlayStatus(VPPS_Play);
}
