#include "VideoToolBarWidget.h"
#include "BmpButton.h"
#include "Slider.h"
#include "BusinessLogic/Multimedia.h"
#include "UserInterfaceUtility.h"
#include "AutoConnect.h"
#include "BusinessLogic/BusinessLogicUtility.h"
#include "TextWidget.h"
#include "UserInterfaceUtility.h"
#include "BmpWidget.h"
#include <QPainter>
#include <QVariant>
#include <QTimer>

class VideoToolBarWidgetPrivate
{
    Q_DISABLE_COPY(VideoToolBarWidgetPrivate)
public:
    explicit VideoToolBarWidgetPrivate(VideoToolBarWidget* parent);
    ~VideoToolBarWidgetPrivate();
    void initialize();
    void connectAllSlots();
    BmpWidget* m_Background;
    BmpButton* m_PreviousBtn;
    BmpButton* m_ToggleBtn;
    BmpButton* m_NextBtn;
    BmpButton* m_ModeBtn;
    BmpButton* m_ListBtn;
private:
    VideoToolBarWidget* m_Parent;
};

VideoToolBarWidget::VideoToolBarWidget(QWidget *parent)
    : QWidget(parent)
    , m_Private(new VideoToolBarWidgetPrivate(this))
{
    setVisible(true);
}

VideoToolBarWidget::~VideoToolBarWidget()
{
}

void VideoToolBarWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(0, 387, 800, 94, this);
}

void VideoToolBarWidget::onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status)
{
    switch (destinationType) {
    case Widget::T_SDVideo:
    case Widget::T_USBVideo: {
        if (WidgetStatus::Show == status) {
            setVisible(true);
        }
        break;
    }
    case Widget::T_SDVideoFullScreen:
    case Widget::T_USBVideoFullScreen: {
        if (WidgetStatus::Show == status) {
            setVisible(false);
        }
        break;
    }
    default : {
        break;
    }
    }
}

void VideoToolBarWidget::onVideoPlayerPlayStatus(const DeviceWatcherType type, const VideoPlayerPlayStatus status)
{
    m_Private->m_PreviousBtn->setEnabled(VPPS_Start != status);
    m_Private->m_ToggleBtn->setEnabled(VPPS_Start != status);
    m_Private->m_NextBtn->setEnabled(VPPS_Start != status);
    switch (status) {
    case VPPS_Play: {
        m_Private->m_ToggleBtn->setNormalBmpPath(QString(":/Images/hw_01_multimedia_pause_normal.png"));
        m_Private->m_ToggleBtn->setPressBmpPath(QString(":/Images/hw_01_multimedia_pause_pressed.png"));
        break;
    }
    case VPPS_Start:
    case VPPS_Unsupport:
    case VPPS_Stop:
    case VPPS_Pause: {
        m_Private->m_ToggleBtn->setNormalBmpPath(QString(":/Images/hw_01_multimedia_play_normal.png"));
        m_Private->m_ToggleBtn->setPressBmpPath(QString(":/Images/hw_01_multimedia_play_pressed.png"));
        break;
    }
    default : {
        break;
    }
    }
}

void VideoToolBarWidget::onVideoPlayerPlayMode(const VideoPlayerPlayMode mode)
{
    switch (mode) {
    case VPPM_RepeatOnce: {
        m_Private->m_ModeBtn->setNormalBmpPath(QString(":/Images/hw_01_multimedia_playmode_single_normal.png"));
        m_Private->m_ModeBtn->setPressBmpPath(QString(":/Images/hw_01_multimedia_playmode_single_pressed.png"));
        break;
    }
    case VPPM_Shuffle: {
        m_Private->m_ModeBtn->setNormalBmpPath(QString(":/Images/hw_01_multimedia_playmode_random_normal.png"));
        m_Private->m_ModeBtn->setPressBmpPath(QString(":/Images/hw_01_multimedia_playmode_random_pressed.png"));
        break;
    }
    case VPPM_AllRepeat:
    default : {
        m_Private->m_ModeBtn->setNormalBmpPath(QString(":/Images/hw_01_multimedia_playmode_list_normal.png"));
        m_Private->m_ModeBtn->setPressBmpPath(QString(":/Images/hw_01_multimedia_playmode_list_pressed.png"));
        break;
    }
    }
}

void VideoToolBarWidget::onToolButtonRelease()
{
    BmpButton* ptr = static_cast<BmpButton*>(sender());
    if (ptr == m_Private->m_PreviousBtn) {
        g_Multimedia->videoPlayerPlayPreviousListViewIndex();
    } else if (ptr == m_Private->m_ToggleBtn) {
        g_Multimedia->videoPlayerSetPlayStatusToggle();
    } else if (ptr == m_Private->m_NextBtn) {
        g_Multimedia->videoPlayerPlayNextListViewIndex();
    } else if (ptr == m_Private->m_ModeBtn) {
        g_Multimedia->videoPlayerSetPlayModeToggle();
    } else if (ptr == m_Private->m_ListBtn) {
        g_Widget->setWidgetType(Widget::T_Back, Widget::T_Undefine, WidgetStatus::RequestShow);
    }
}

VideoToolBarWidgetPrivate::VideoToolBarWidgetPrivate(VideoToolBarWidget *parent)
    : m_Parent(parent)
{
    m_Background = NULL;
    m_PreviousBtn = NULL;
    m_ToggleBtn = NULL;
    m_NextBtn = NULL;
    m_ModeBtn = NULL;
    m_ListBtn = NULL;
    initialize();
    connectAllSlots();
}

VideoToolBarWidgetPrivate::~VideoToolBarWidgetPrivate()
{
}

void VideoToolBarWidgetPrivate::initialize()
{
    int btnY = 7;
    int btnWidth = 160;
    int btnHeight = 94;

    m_Background = new BmpWidget(m_Parent);
    m_Background->setBackgroundBmpPath(QString(":/Images/hw_01_radio_bottom_toolBar.png"));
    g_Widget->geometryFit(0, btnY, 800, btnHeight, m_Background);
    m_Background->setVisible(true);
    m_PreviousBtn = new BmpButton(m_Parent);
    g_Widget->geometryFit(0, btnY, btnWidth, btnHeight, m_PreviousBtn);
    m_PreviousBtn->setNormalBmpPath(QString(":/Images/hw_01_multimedia_previous_normal.png"));
    m_PreviousBtn->setPressBmpPath(QString(":/Images/hw_01_multimedia_previous_pressed.png"));
    m_PreviousBtn->setVisible(true);
    m_ToggleBtn = new BmpButton(m_Parent);
    g_Widget->geometryFit(btnWidth+1, btnY, btnWidth-2, btnHeight, m_ToggleBtn);
    m_ToggleBtn->setNormalBmpPath(QString(":/Images/hw_01_multimedia_play_normal.png"));
    m_ToggleBtn->setPressBmpPath(QString(":/Images/hw_01_multimedia_play_pressed.png"));
    m_ToggleBtn->setVisible(true);
    m_NextBtn = new BmpButton(m_Parent);
    g_Widget->geometryFit(btnWidth*2+2, btnY, btnWidth-3, btnHeight, m_NextBtn);
    m_NextBtn->setNormalBmpPath(QString(":/Images/hw_01_multimedia_next_normal.png"));
    m_NextBtn->setPressBmpPath(QString(":/Images/hw_01_multimedia_next_pressed.png"));
    m_NextBtn->setVisible(true);
    m_ModeBtn = new BmpButton(m_Parent);
    g_Widget->geometryFit(btnWidth*3+2, btnY, btnWidth-2, btnHeight, m_ModeBtn);
    m_ModeBtn->setNormalBmpPath(QString(":/Images/hw_01_multimedia_playmode_normal.png"));
    m_ModeBtn->setPressBmpPath(QString(":/Images/hw_01_multimedia_playmode_pressed.png"));
    m_ModeBtn->setVisible(true);
    m_ListBtn = new BmpButton(m_Parent);
    g_Widget->geometryFit(btnWidth*4+2, btnY, btnWidth-2, btnHeight, m_ListBtn);
    m_ListBtn->setNormalBmpPath(QString(":/Images/hw_01_multimedia_function_normal.png"));
    m_ListBtn->setPressBmpPath(QString(":/Images/hw_01_multimedia_function_pressed.png"));
    m_ListBtn->setVisible(true);
}

void VideoToolBarWidgetPrivate::connectAllSlots()
{
    connectSignalAndSlotByNamesake(g_Widget, m_Parent, ARKRECEIVER(onWidgetTypeChange(const Widget::Type, const Widget::Type, const QString &)));
    connectSignalAndSlotByNamesake(g_Multimedia, m_Parent, ARKRECEIVER(onVideoPlayerPlayStatus(const int, const int)));
    connectSignalAndSlotByNamesake(g_Multimedia, m_Parent, ARKRECEIVER(onVideoPlayerPlayMode(const int)));
    Qt::ConnectionType type = static_cast<Qt::ConnectionType>(Qt::AutoConnection | Qt::UniqueConnection);
    QObject::connect(m_PreviousBtn, ARKSENDER(released()),
                     m_Parent,      ARKRECEIVER(onToolButtonRelease()),
                     type);
    QObject::connect(m_ToggleBtn, ARKSENDER(released()),
                     m_Parent,    ARKRECEIVER(onToolButtonRelease()),
                     type);
    QObject::connect(m_NextBtn, ARKSENDER(released()),
                     m_Parent,  ARKRECEIVER(onToolButtonRelease()),
                     type);
    QObject::connect(m_ModeBtn, ARKSENDER(released()),
                     m_Parent,  ARKRECEIVER(onToolButtonRelease()),
                     type);
    QObject::connect(m_ListBtn, ARKSENDER(released()),
                     m_Parent,  ARKRECEIVER(onToolButtonRelease()),
                     type);
}
