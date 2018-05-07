#include "HomeWidget.h"
#include "ViewPaperWidget/ViewPaperWidget.h"
#include "AutoConnect.h"
#include "BmpWidget.h"
#include "RadioMiniWidget/RadioMiniWidget.h"
#include "MusicMiniWidget/MusicMiniWidget.h"
#include "BluetoothMusicMiniWidget/BluetoothMusicMiniWidget.h"
#include "BrandMiniWidget/BrandMiniWidget.h"
#include "TimeWidget/TimeWidget.h"
#include "UtilityWidget/UtilityWidget.h"
#include "BusinessLogic/Audio.h"
#include <QPixmap>
#include <QPainter>
#include <QPaintEvent>
#include <QDebug>

class HomeWidgetPrivate
{
    Q_DISABLE_COPY(HomeWidgetPrivate)
public:
    explicit HomeWidgetPrivate(HomeWidget* parent);
    ~HomeWidgetPrivate();
    void initializeParent();
    void initializeBasic();
    void connectAllSlots();
    void initializeBasicWidget();
    void initializeRadioMiniWidget();
    void initializeMusicMiniWidget();
    void initializeBluetoothMusicMiniWidget();
    void initializeBrandMiniWidget();
private:
    Q_DECLARE_PUBLIC(HomeWidget)
    HomeWidget* const q_ptr;
    BmpWidget* m_Background;
    TimeWidget* m_TimeWidget;
    UtilityWidget* m_UtilityWidget;
    RadioMiniWidget* m_RadioMiniWidget;
    MusicMiniWidget* m_MusicMiniWidget;
    BluetoothMusicMiniWidget* m_BluetoothMusicMiniWidget;
    BrandMiniWidget* m_BrandMiniWidget;
    ViewPaperWidget* m_ViewPaperWidget;
};

HomeWidget::HomeWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new HomeWidgetPrivate(this))
{
}

HomeWidget::~HomeWidget()
{
    if (NULL != d_ptr) {
        delete d_ptr;
    }
}

void HomeWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(0, 0, 800, 480, this);
}

void HomeWidget::onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status)
{
    Q_D(HomeWidget);
    switch (destinationType) {
    case Widget::T_ViewPaper0:
    case Widget::T_ViewPaper1: {
        if (WidgetStatus::Show == status) {
            setVisible(true);
        }
        break;
    }
    case Widget::T_Radio:
    case Widget::T_Bluetooth:
    case Widget::T_BluetoothDial:
    case Widget::T_BluetoothContact:
    case Widget::T_BluetoothRecord:
    case Widget::T_BluetoothMusic:
    case Widget::T_BluetoothSetting:
    case Widget::T_SDDisk:
    case Widget::T_SDDiskMusic:
    case Widget::T_SDDiskImage:
    case Widget::T_SDDiskVideo:
    case Widget::T_USBDisk:
    case Widget::T_USBDiskMusic:
    case Widget::T_USBDiskImage:
    case Widget::T_USBDiskVideo:
    case Widget::T_SDImage:
    case Widget::T_SDImageFullScreen:
    case Widget::T_USBImage:
    case Widget::T_USBImageFullScreen:
    case Widget::T_SDMusic:
    case Widget::T_USBMusic:
    case Widget::T_SDVideo:
    case Widget::T_SDVideoFullScreen:
    case Widget::T_USBVideo:
    case Widget::T_USBVideoFullScreen:
    case Widget::T_Link:
    case Widget::T_Carlife:
    case Widget::T_Auto:
    case Widget::T_Carplay:
    case Widget::T_AUX:
    case Widget::T_Camera:
    case Widget::T_Steering:
    case Widget::T_Setting:
    case Widget::T_SettingGeneral:
    case Widget::T_SettingStyle:
    case Widget::T_SettingSound:
    case Widget::T_SettingFactory: {
        if (WidgetStatus::Show == status) {
            if (isVisible()) {
                setVisible(false);
            }
        }
        break;
    }
    default: {
        break;
    }
    }
}

void HomeWidget::onHolderChange(const AudioSource oldHolder, const AudioSource newHolder)
{
    Q_D(HomeWidget);
    switch (newHolder) {
    case AS_Radio: {
        if (NULL != d->m_BrandMiniWidget) {
            d->m_BrandMiniWidget->setVisible(false);
        }
        if (NULL != d->m_BluetoothMusicMiniWidget) {
            d->m_BluetoothMusicMiniWidget->setVisible(false);
        }
        if (NULL != d->m_MusicMiniWidget) {
            d->m_MusicMiniWidget->setVisible(false);
        }
        d->initializeRadioMiniWidget();
        d->m_RadioMiniWidget->setVisible(true);
        break;
    }
    case AS_Music: {
        if (NULL != d->m_BrandMiniWidget) {
            d->m_BrandMiniWidget->setVisible(false);
        }
        if (NULL != d->m_BluetoothMusicMiniWidget) {
            d->m_BluetoothMusicMiniWidget->setVisible(false);
        }
        if (NULL != d->m_RadioMiniWidget) {
            d->m_RadioMiniWidget->setVisible(false);
        }
        d->initializeMusicMiniWidget();
        d->m_MusicMiniWidget->setVisible(true);
        break;
    }
    case AS_BluetoothMusic: {
        if (NULL != d->m_BrandMiniWidget) {
            d->m_BrandMiniWidget->setVisible(false);
        }
        if (NULL != d->m_MusicMiniWidget) {
            d->m_MusicMiniWidget->setVisible(false);
        }
        if (NULL != d->m_RadioMiniWidget) {
            d->m_RadioMiniWidget->setVisible(false);
        }
        d->initializeBluetoothMusicMiniWidget();
        d->m_BluetoothMusicMiniWidget->setVisible(true);
        break;
    }
    default: {
//        if (NULL != d->m_BluetoothMusicMiniWidget) {
//            d->m_BluetoothMusicMiniWidget->setVisible(false);
//        }
//        if (NULL != d->m_MusicMiniWidget) {
//            d->m_MusicMiniWidget->setVisible(false);
//        }
//        if (NULL != d->m_RadioMiniWidget) {
//            d->m_RadioMiniWidget->setVisible(false);
//        }
//        d->initializeBrandMiniWidget();
//        d->m_BrandMiniWidget->setVisible(true);
        if (NULL != d->m_BrandMiniWidget) {
            d->m_BrandMiniWidget->setVisible(false);
        }
        if (NULL != d->m_BluetoothMusicMiniWidget) {
            d->m_BluetoothMusicMiniWidget->setVisible(false);
        }
        if (NULL != d->m_RadioMiniWidget) {
            d->m_RadioMiniWidget->setVisible(false);
        }
        d->initializeMusicMiniWidget();
        d->m_MusicMiniWidget->setVisible(true);
        break;
    }
    }
}

HomeWidgetPrivate::HomeWidgetPrivate(HomeWidget *parent)
    : q_ptr(parent)
{
    m_Background = NULL;
    m_TimeWidget = NULL;
    m_UtilityWidget = NULL;
    m_RadioMiniWidget = NULL;
    m_MusicMiniWidget = NULL;
    m_BluetoothMusicMiniWidget = NULL;
    m_BrandMiniWidget = NULL;
    m_ViewPaperWidget = NULL;
    initializeParent();
    initializeBasic();
    connectAllSlots();
    initializeBasicWidget();
}

HomeWidgetPrivate::~HomeWidgetPrivate()
{
}

void HomeWidgetPrivate::initializeParent()
{
    Q_Q(HomeWidget);
    q->setVisible(false);
}

void HomeWidgetPrivate::initializeBasic()
{
    Q_Q(HomeWidget);
    m_Background = new BmpWidget(q);
//    m_Background->setBackgroundBmpPath(QString(":/Images/HomeWidgetBackground.png"));
//    g_Widget->geometryFit(0, 302, 800, 178, m_Background);
//    m_Background->setVisible(true);
}

void HomeWidgetPrivate::connectAllSlots()
{
    Q_Q(HomeWidget);
    connectSignalAndSlotByNamesake(g_Widget, q, ARKRECEIVER(onWidgetTypeChange(const Widget::Type, const Widget::Type, const QString &)));
    connectSignalAndSlotByNamesake(g_Audio, q, ARKRECEIVER(onHolderChange(const int, const int)));
}

void HomeWidgetPrivate::initializeBasicWidget()
{
    Q_Q(HomeWidget);
    m_ViewPaperWidget = new ViewPaperWidget(q);
    m_TimeWidget = new TimeWidget(q);
//    m_UtilityWidget = new UtilityWidget(q);
    AudioSource audio = g_Audio->getAudioSource();
    switch (audio) {
    case AS_Radio: {
        initializeRadioMiniWidget();
        break;
    }
    case AS_Music: {
        initializeMusicMiniWidget();
        break;
    }
    case AS_BluetoothMusic: {
        initializeBluetoothMusicMiniWidget();
        break;
    }
    default: {
//        initializeBrandMiniWidget();
        initializeMusicMiniWidget();
        break;
    }
    }
}

void HomeWidgetPrivate::initializeRadioMiniWidget()
{
    if (NULL == m_RadioMiniWidget) {
        Q_Q(HomeWidget);
        m_RadioMiniWidget = new RadioMiniWidget(q);
        m_RadioMiniWidget->setVisible(true);
    }
}

void HomeWidgetPrivate::initializeMusicMiniWidget()
{
    if (NULL == m_MusicMiniWidget) {
        Q_Q(HomeWidget);
        m_MusicMiniWidget = new MusicMiniWidget(q);
        m_MusicMiniWidget->setVisible(true);
    }
}

void HomeWidgetPrivate::initializeBluetoothMusicMiniWidget()
{
    if (NULL == m_BluetoothMusicMiniWidget) {
        Q_Q(HomeWidget);
        m_BluetoothMusicMiniWidget = new BluetoothMusicMiniWidget(q);
        m_BluetoothMusicMiniWidget->setVisible(true);
    }
}

void HomeWidgetPrivate::initializeBrandMiniWidget()
{
    if (NULL == m_BrandMiniWidget) {
        Q_Q(HomeWidget);
        m_BrandMiniWidget = new BrandMiniWidget(q);
        m_BrandMiniWidget->setVisible(true);
    }
}
