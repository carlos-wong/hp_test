#include "StatusBarWidget.h"
#include "AutoConnect.h"
#include "TextWidget.h"
#include "BmpButton.h"
#include "BmpWidget.h"
#include "BusinessLogic/Setting.h"
#include "UserInterfaceUtility.h"
#include <QPainter>
#include <QPixmap>
#include <QPaintEvent>
#include <QDate>
#include <QDebug>

namespace SourceString {
static const QString Radio(QObject::tr("Radio"));
static const QString Bluetooth(QObject::tr("Bluetooth"));
static const QString SD(QObject::tr("SD"));
static const QString USB(QObject::tr("USB"));
static const QString SDMusic(QObject::tr("SDMusic"));
static const QString USBMusic(QObject::tr("USBMusic"));
static const QString SDImage(QObject::tr("SDImage"));
static const QString USBImage(QObject::tr("USBImage"));
static const QString SDVideo(QObject::tr("SDVideo"));
static const QString USBVideo(QObject::tr("USBVideo"));
static const QString Link(QObject::tr("Link"));
static const QString AUX(QObject::tr("AUX"));
static const QString Camera(QObject::tr("Camera"));
static const QString Steering(QObject::tr("Steering"));
static const QString Setting(QObject::tr("Setting"));
}

class StatusBarWidgetPrivate
{
    Q_DISABLE_COPY(StatusBarWidgetPrivate)
public:
    explicit StatusBarWidgetPrivate(StatusBarWidget* parent);
    ~StatusBarWidgetPrivate();
    void initializeParent();
    void initializeBasic();
    void initializeBasicWidget();
    void initializeDate();
    void initializeTitle();
    void initializeTime();
    void initializeHomeButton();
    void initializeBackButton();
private:
    Q_DECLARE_PUBLIC(StatusBarWidget)
    BmpWidget* m_Background;
    TextWidget* m_DateText;
    BmpButton* m_HomeButton;
    BmpButton* m_BackButton;
    TextWidget* m_Title;
    TextWidget* m_Time;
    BmpWidget* m_BluetoothTip;
    BmpWidget* m_SDTip;
    BmpWidget* m_USBTip;
private:
    StatusBarWidget* const q_ptr;
};

StatusBarWidget::StatusBarWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new StatusBarWidgetPrivate(this))
{
}

StatusBarWidget::~StatusBarWidget()
{
    if (NULL != d_ptr) {
        delete d_ptr;
    }
}

void StatusBarWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(0, 0, 800, 52, this);
}

void StatusBarWidget::onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status)
{
    qDebug() << __PRETTY_FUNCTION__ << __LINE__ << destinationType << requestType << status;
    Q_D(StatusBarWidget);
    switch (destinationType) {
    case Widget::T_ViewPaper0:
    case Widget::T_ViewPaper1: {
        if (WidgetStatus::Show == status) {
            d->initializeTitle();
            d->m_Title->setVisible(false);
            d->initializeTime();
            d->m_Time->setVisible(false);
            d->initializeHomeButton();
            d->initializeBackButton();
            d->initializeDate();
            d->m_DateText->setText(QDate::currentDate().toString(QString("yyyy/MM/dd")));
            d->m_DateText->setVisible(true);
            g_Widget->geometryFit(680, 0, 40, 40, d->m_BluetoothTip);
            g_Widget->geometryFit(720, 0, 40, 40, d->m_SDTip);
            g_Widget->geometryFit(760, 0, 40, 40, d->m_USBTip);
            d->m_HomeButton->setVisible(false);
            d->m_BackButton->setVisible(false);
            setVisible(true);
        }
        break;
    }
    case Widget::T_Radio: {
        if (WidgetStatus::Show == status) {
            raise();
            d->initializeTitle();
            d->m_Title->setText(SourceString::Radio);
            d->m_Title->setVisible(true);
            d->initializeTime();
            d->m_Time->setVisible(true);
            d->initializeHomeButton();
            d->initializeBackButton();
            if (NULL != d->m_DateText) {
                d->m_DateText->setVisible(false);
            }
            g_Widget->geometryFit(604, 0, 40, 40, d->m_BluetoothTip);
            g_Widget->geometryFit(644, 0, 40, 40, d->m_SDTip);
            g_Widget->geometryFit(684, 0, 40, 40, d->m_USBTip);
            d->m_HomeButton->setVisible(true);
            d->m_BackButton->setVisible(true);
            setVisible(true);
        }
        break;
    }
    case Widget::T_Bluetooth:
    case Widget::T_BluetoothDial:
    case Widget::T_BluetoothContact:
    case Widget::T_BluetoothRecord:
    case Widget::T_BluetoothMusic:
    case Widget::T_BluetoothSetting: {
        if (WidgetStatus::Show == status) {
            raise();
            d->initializeTitle();
            d->m_Title->setText(SourceString::Bluetooth);
            d->m_Title->setVisible(true);
            d->initializeTime();
            d->m_Time->setVisible(true);
            d->initializeHomeButton();
            d->initializeBackButton();
            if (NULL != d->m_DateText) {
                if (d->m_DateText->isVisible()) {
                    d->m_DateText->setVisible(false);
                }
            }
            g_Widget->geometryFit(604, 0, 40, 40, d->m_BluetoothTip);
            g_Widget->geometryFit(644, 0, 40, 40, d->m_SDTip);
            g_Widget->geometryFit(684, 0, 40, 40, d->m_USBTip);
            if (!d->m_HomeButton->isVisible()) {
                d->m_HomeButton->setVisible(true);
            }
            if (!d->m_BackButton->isVisible()) {
                d->m_BackButton->setVisible(true);
            }
            if (!isVisible()) {
                setVisible(true);
            }
        }
        break;
    }
    case Widget::T_SDDisk:
    case Widget::T_SDDiskMusic:
    case Widget::T_SDDiskImage:
    case Widget::T_SDDiskVideo: {
        if (WidgetStatus::Show == status) {
            raise();
            d->initializeTitle();
            d->m_Title->setText(SourceString::SD);
            d->m_Title->setVisible(true);
            d->initializeTime();
            d->m_Time->setVisible(true);
            d->initializeHomeButton();
            d->initializeBackButton();
            if (NULL != d->m_DateText) {
                d->m_DateText->setVisible(false);
            }
            g_Widget->geometryFit(604, 0, 40, 40, d->m_BluetoothTip);
            g_Widget->geometryFit(644, 0, 40, 40, d->m_SDTip);
            g_Widget->geometryFit(684, 0, 40, 40, d->m_USBTip);
            d->m_HomeButton->setVisible(true);
            d->m_BackButton->setVisible(true);
            setVisible(true);
        }
        break;
    }
    case Widget::T_USBDisk:
    case Widget::T_USBDiskMusic:
    case Widget::T_USBDiskImage:
    case Widget::T_USBDiskVideo: {
        if (WidgetStatus::Show == status) {
            raise();
            d->initializeTitle();
            d->m_Title->setText(SourceString::USB);
            d->m_Title->setVisible(true);
            d->initializeTime();
            d->m_Time->setVisible(true);
            d->initializeHomeButton();
            d->initializeBackButton();
            if (NULL != d->m_DateText) {
                d->m_DateText->setVisible(false);
            }
            g_Widget->geometryFit(604, 0, 40, 40, d->m_BluetoothTip);
            g_Widget->geometryFit(644, 0, 40, 40, d->m_SDTip);
            g_Widget->geometryFit(684, 0, 40, 40, d->m_USBTip);
            d->m_HomeButton->setVisible(true);
            d->m_BackButton->setVisible(true);
            setVisible(true);
        }
        break;
    }
    case Widget::T_SDMusic: {
        if (WidgetStatus::Show == status) {
            raise();
            d->initializeTitle();
            d->m_Title->setText(SourceString::SDMusic);
            d->m_Title->setVisible(true);
            d->initializeTime();
            d->m_Time->setVisible(true);
            d->initializeHomeButton();
            d->initializeBackButton();
            if (NULL != d->m_DateText) {
                d->m_DateText->setVisible(false);
            }
            g_Widget->geometryFit(604, 0, 40, 40, d->m_BluetoothTip);
            g_Widget->geometryFit(644, 0, 40, 40, d->m_SDTip);
            g_Widget->geometryFit(684, 0, 40, 40, d->m_USBTip);
            d->m_HomeButton->setVisible(true);
            d->m_BackButton->setVisible(true);
            setVisible(true);
        }
        break;
    }
    case Widget::T_USBMusic: {
        if (WidgetStatus::Show == status) {
            raise();
            d->initializeTitle();
            d->m_Title->setText(SourceString::USBMusic);
            d->m_Title->setVisible(true);
            d->initializeTime();
            d->m_Time->setVisible(true);
            d->initializeHomeButton();
            d->initializeBackButton();
            if (NULL != d->m_DateText) {
                d->m_DateText->setVisible(false);
            }
            g_Widget->geometryFit(604, 0, 40, 40, d->m_BluetoothTip);
            g_Widget->geometryFit(644, 0, 40, 40, d->m_SDTip);
            g_Widget->geometryFit(684, 0, 40, 40, d->m_USBTip);
            d->m_HomeButton->setVisible(true);
            d->m_BackButton->setVisible(true);
            setVisible(true);
        }
        break;
    }
    case Widget::T_SDImage: {
        if (WidgetStatus::Show == status) {
            raise();
            d->initializeTitle();
            d->m_Title->setText(SourceString::SDImage);
            d->m_Title->setVisible(true);
            d->initializeTime();
            d->m_Time->setVisible(true);
            d->initializeHomeButton();
            d->initializeBackButton();
            if (NULL != d->m_DateText) {
                d->m_DateText->setVisible(false);
            }
            g_Widget->geometryFit(604, 0, 40, 40, d->m_BluetoothTip);
            g_Widget->geometryFit(644, 0, 40, 40, d->m_SDTip);
            g_Widget->geometryFit(684, 0, 40, 40, d->m_USBTip);
            d->m_HomeButton->setVisible(true);
            d->m_BackButton->setVisible(true);
            setVisible(true);
        }
        break;
    }
    case Widget::T_USBImage: {
        if (WidgetStatus::Show == status) {
            raise();
            d->initializeTitle();
            d->m_Title->setText(SourceString::USBImage);
            d->m_Title->setVisible(true);
            d->initializeTime();
            d->m_Time->setVisible(true);
            d->initializeHomeButton();
            d->initializeBackButton();
            if (NULL != d->m_DateText) {
                d->m_DateText->setVisible(false);
            }
            g_Widget->geometryFit(604, 0, 40, 40, d->m_BluetoothTip);
            g_Widget->geometryFit(644, 0, 40, 40, d->m_SDTip);
            g_Widget->geometryFit(684, 0, 40, 40, d->m_USBTip);
            d->m_HomeButton->setVisible(true);
            d->m_BackButton->setVisible(true);
            setVisible(true);
        }
        break;
    }
    case Widget::T_SDVideo: {
        if (WidgetStatus::Show == status) {
            raise();
            d->initializeTitle();
            d->m_Title->setText(SourceString::SDVideo);
            d->m_Title->setVisible(true);
            d->initializeTime();
            d->m_Time->setVisible(true);
            d->initializeHomeButton();
            d->initializeBackButton();
            if (NULL != d->m_DateText) {
                d->m_DateText->setVisible(false);
            }
            g_Widget->geometryFit(604, 0, 40, 40, d->m_BluetoothTip);
            g_Widget->geometryFit(644, 0, 40, 40, d->m_SDTip);
            g_Widget->geometryFit(684, 0, 40, 40, d->m_USBTip);
            d->m_HomeButton->setVisible(true);
            d->m_BackButton->setVisible(true);
            setVisible(true);
        }
        break;
    }
    case Widget::T_USBVideo: {
        if (WidgetStatus::Show == status) {
            raise();
            d->initializeTitle();
            d->m_Title->setText(SourceString::USBVideo);
            d->m_Title->setVisible(true);
            d->initializeTime();
            d->m_Time->setVisible(true);
            d->initializeHomeButton();
            d->initializeBackButton();
            if (NULL != d->m_DateText) {
                d->m_DateText->setVisible(false);
            }
            g_Widget->geometryFit(604, 0, 40, 40, d->m_BluetoothTip);
            g_Widget->geometryFit(644, 0, 40, 40, d->m_SDTip);
            g_Widget->geometryFit(684, 0, 40, 40, d->m_USBTip);
            d->m_HomeButton->setVisible(true);
            d->m_BackButton->setVisible(true);
            setVisible(true);
        }
        break;
    }
    case Widget::T_Link: {
        if (WidgetStatus::Show == status) {
            raise();
            d->initializeTitle();
            d->m_Title->setText(SourceString::Link);
            d->m_Title->setVisible(true);
            d->initializeTime();
            d->m_Time->setVisible(true);
            d->initializeHomeButton();
            d->initializeBackButton();
            if (NULL != d->m_DateText) {
                d->m_DateText->setVisible(false);
            }
            g_Widget->geometryFit(604, 0, 40, 40, d->m_BluetoothTip);
            g_Widget->geometryFit(644, 0, 40, 40, d->m_SDTip);
            g_Widget->geometryFit(684, 0, 40, 40, d->m_USBTip);
            d->m_HomeButton->setVisible(true);
            d->m_BackButton->setVisible(true);
            setVisible(true);
        }
        break;
    }
    case Widget::T_AUX: {
        if (WidgetStatus::Show == status) {
            raise();
            d->initializeTitle();
            d->m_Title->setText(SourceString::AUX);
            d->m_Title->setVisible(true);
            d->initializeTime();
            d->m_Time->setVisible(true);
            d->initializeHomeButton();
            d->initializeBackButton();
            if (NULL != d->m_DateText) {
                d->m_DateText->setVisible(false);
            }
            g_Widget->geometryFit(604, 0, 40, 40, d->m_BluetoothTip);
            g_Widget->geometryFit(644, 0, 40, 40, d->m_SDTip);
            g_Widget->geometryFit(684, 0, 40, 40, d->m_USBTip);
            d->m_HomeButton->setVisible(true);
            d->m_BackButton->setVisible(true);
            setVisible(true);
        }
        break;
    }
    case Widget::T_Camera: {
        if (WidgetStatus::Show == status) {
            raise();
            d->initializeTitle();
            d->m_Title->setText(SourceString::Camera);
            d->m_Title->setVisible(true);
            d->initializeTime();
            d->m_Time->setVisible(true);
            d->initializeHomeButton();
            d->initializeBackButton();
            if (NULL != d->m_DateText) {
                d->m_DateText->setVisible(false);
            }
            g_Widget->geometryFit(604, 0, 40, 40, d->m_BluetoothTip);
            g_Widget->geometryFit(644, 0, 40, 40, d->m_SDTip);
            g_Widget->geometryFit(684, 0, 40, 40, d->m_USBTip);
            d->m_HomeButton->setVisible(true);
            d->m_BackButton->setVisible(true);
            setVisible(true);
        }
        break;
    }
    case Widget::T_Steering: {
        if (WidgetStatus::Show == status) {
            raise();
            d->initializeTitle();
            d->m_Title->setText(SourceString::Steering);
            d->m_Title->setVisible(true);
            d->initializeTime();
            d->m_Time->setVisible(true);
            d->initializeHomeButton();
            d->initializeBackButton();
            if (NULL != d->m_DateText) {
                d->m_DateText->setVisible(false);
            }
            g_Widget->geometryFit(604, 0, 40, 40, d->m_BluetoothTip);
            g_Widget->geometryFit(644, 0, 40, 40, d->m_SDTip);
            g_Widget->geometryFit(684, 0, 40, 40, d->m_USBTip);
            d->m_HomeButton->setVisible(true);
            d->m_BackButton->setVisible(true);
            setVisible(true);
        }
        break;
    }
    case Widget::T_AUXFullScreen:
    case Widget::T_CameraFullScreen:
    case Widget::T_SDImageFullScreen:
    case Widget::T_USBImageFullScreen:
    case Widget::T_SDVideoFullScreen:
    case Widget::T_USBVideoFullScreen:
    case Widget::T_Carlife:
    case Widget::T_Auto:
    case Widget::T_Carplay: {
        if (WidgetStatus::Show == status) {
            setVisible(false);
        }
        break;
    }
    case Widget::T_SettingGeneral: {
        if (WidgetStatus::Show == status) {
            raise();
            d->initializeTitle();
            d->m_Title->setText(SourceString::Setting);
            d->m_Title->setVisible(true);
            d->initializeTime();
            d->m_Time->setVisible(true);
            d->initializeHomeButton();
            d->initializeBackButton();
            if (NULL != d->m_DateText) {
                d->m_DateText->setVisible(false);
            }
            g_Widget->geometryFit(604, 0, 40, 40, d->m_BluetoothTip);
            g_Widget->geometryFit(644, 0, 40, 40, d->m_SDTip);
            g_Widget->geometryFit(684, 0, 40, 40, d->m_USBTip);
            d->m_HomeButton->setVisible(true);
            d->m_BackButton->setVisible(true);
            setVisible(true);
            connectSignalAndSlotByNamesake(g_Setting, this, ARKRECEIVER(onHourClockChange(const int)));
        }
        break;
    }
    case Widget::T_Setting:
    case Widget::T_SettingStyle:
    case Widget::T_SettingSound:
    case Widget::T_SettingFactory: {
        if (WidgetStatus::Show == status) {
            raise();
            d->initializeTitle();
            d->m_Title->setText(SourceString::Setting);
            d->m_Title->setVisible(true);
            d->initializeTime();
            d->m_Time->setVisible(true);
            d->initializeHomeButton();
            d->initializeBackButton();
            if (NULL != d->m_DateText) {
                d->m_DateText->setVisible(false);
            }
            g_Widget->geometryFit(604, 0, 40, 40, d->m_BluetoothTip);
            g_Widget->geometryFit(644, 0, 40, 40, d->m_SDTip);
            g_Widget->geometryFit(684, 0, 40, 40, d->m_USBTip);
            d->m_HomeButton->setVisible(true);
            d->m_BackButton->setVisible(true);
            setVisible(true);
        }
        break;
    }
    default: {
        break;
    }
    }
}

void StatusBarWidget::onDeviceWatcherStatus(const DeviceWatcherType type, const DeviceWatcherStatus status)
{
    Q_D(StatusBarWidget);
    switch (type) {
    case DWT_SDDisk: {
        if ((DWS_Busy == status)
                || (DWS_Ready == status)
                || (DWS_Unsupport == status)) {
            d->m_SDTip->setBackgroundBmpPath(QString(":/Images/StatusBarWidgetSDTipEnable.png"));
        } else if (DWS_Remove == status){
            d->m_SDTip->setBackgroundBmpPath(QString(":/Images/StatusBarWidgetSDTipDisable.png"));
        }
        break;
    }
    case DWT_USBDisk: {
        if ((DWS_Busy == status)
                || (DWS_Ready == status)
                || (DWS_Unsupport == status)) {
            d->m_USBTip->setBackgroundBmpPath(QString(":/Images/StatusBarWidgetUSBTipEnable.png"));
        } else if (DWS_Remove == status){
            d->m_USBTip->setBackgroundBmpPath(QString(":/Images/StatusBarWidgetUSBTipDisable.png"));
        }
        break;
        break;
    }
    default: {
        break;
    }
    }
}

void StatusBarWidget::onConnectStatusChange(const BluetoothConnectStatus status)
{
    Q_D(StatusBarWidget);
    if ((BCS_Idle == status)
            || (BCS_Connecting == status)) {
        d->m_BluetoothTip->setBackgroundBmpPath(QString(":/Images/StatusBarWidgetBluetoothDisable.png"));
    } else {
        d->m_BluetoothTip->setBackgroundBmpPath(QString(":/Images/StatusBarWidgetBluetoothEnable.png"));
    }
}

void StatusBarWidget::onCurrentDateTime(const QDateTime &dateTime)
{
    Q_D(StatusBarWidget);
    if ((NULL != d->m_Time)
            && (d->m_Time->isVisible())) {
        QDateTime dateTime = QDateTime::currentDateTime();
        if (HCT_12HourClock == SettingPersistent::getHourClockType(true)) {
            d->m_Time->setText(dateTime.toString(QString("hh:mmA")));
        } else {
            d->m_Time->setText(dateTime.toString(QString("hh:mm")));
        }
    }
    if ((NULL != d->m_DateText)
            && (d->m_DateText->isVisible())) {
        d->m_DateText->setText(dateTime.date().toString(QString("yyyy/MM/dd")));
    }
}

void StatusBarWidget::onHourClockChange(const int type)
{
    Q_D(StatusBarWidget);
    QDateTime dateTime = QDateTime::currentDateTime();
    if (HCT_12HourClock == type) {
        d->m_Time->setText(dateTime.toString(QString("hh:mmA")));
    } else {
        d->m_Time->setText(dateTime.toString(QString("hh:mm")));
    }
}

void StatusBarWidget::onBmpButtonRelease()
{
    BmpButton* ptr = static_cast<BmpButton*>(sender());
    if (NULL != ptr) {
        Q_D(StatusBarWidget);
        if (ptr == d->m_HomeButton) {
            g_Widget->setWidgetType(Widget::T_Home, Widget::T_Undefine, WidgetStatus::RequestShow);
        } else if (ptr == d->m_BackButton) {
            g_Widget->setWidgetType(Widget::T_Back, Widget::T_Undefine, WidgetStatus::RequestShow);
        }
    }
}

StatusBarWidgetPrivate::StatusBarWidgetPrivate(StatusBarWidget *parent)
    : q_ptr(parent)
{
    m_Background = NULL;
    m_DateText = NULL;
    m_HomeButton = NULL;
    m_BackButton = NULL;
    m_Title = NULL;
    m_Time = NULL;
    m_BluetoothTip = NULL;
    m_SDTip = NULL;
    m_USBTip = NULL;
    initializeBasic();
    initializeBasicWidget();
    initializeParent();
}

StatusBarWidgetPrivate::~StatusBarWidgetPrivate()
{
}

void StatusBarWidgetPrivate::initializeParent()
{
    Q_Q(StatusBarWidget);
    q->setVisible(true);
}

void StatusBarWidgetPrivate::initializeBasic()
{
    Q_Q(StatusBarWidget);
    connectSignalAndSlotByNamesake(g_Widget, q, ARKRECEIVER(onWidgetTypeChange(const Widget::Type, const Widget::Type, const QString &)));
    connectSignalAndSlotByNamesake(g_Multimedia, q, ARKRECEIVER(onDeviceWatcherStatus(const int, const int)));
    connectSignalAndSlotByNamesake(g_Bluetooth, q, ARKRECEIVER(onConnectStatusChange(const BluetoothConnectStatus)));
    QObject::connect(ArkApp, ARKSENDER(currentDateTime(const QDateTime&)),
                     q,      ARKRECEIVER(onCurrentDateTime(const QDateTime&)));
}

void StatusBarWidgetPrivate::initializeBasicWidget()
{
    Q_Q(StatusBarWidget);
    m_Background = new BmpWidget(q);
    m_Background->setBackgroundBmpPath(QString(":/Images/StatusBarWidgetBackground.png"));
    g_Widget->geometryFit(0, 0, 800, 52, m_Background);
    m_Background->setVisible(true);

    initializeTitle();
    m_Title->setText(SourceString::Radio);
    m_Title->setVisible(true);
    initializeTime();
    m_Time->setVisible(true);
    initializeHomeButton();
    m_HomeButton->setVisible(true);
    m_BluetoothTip = new BmpWidget(q);
    g_Widget->geometryFit(604, 0, 40, 40, m_BluetoothTip);
    if ((BCS_Idle == g_Bluetooth->connectStatus())
            || (BCS_Connecting == g_Bluetooth->connectStatus())) {
        m_BluetoothTip->setBackgroundBmpPath(QString(":/Images/StatusBarWidgetBluetoothDisable.png"));
    } else {
        m_BluetoothTip->setBackgroundBmpPath(QString(":/Images/StatusBarWidgetBluetoothEnable.png"));
    }
    m_BluetoothTip->setVisible(true);
    m_SDTip = new BmpWidget(q);
    g_Widget->geometryFit(644, 0, 40, 40, m_SDTip);
    m_SDTip->setBackgroundBmpPath(QString(":/Images/StatusBarWidgetSDTipDisable.png"));
    m_SDTip->setVisible(true);
    m_USBTip = new BmpWidget(q);
    g_Widget->geometryFit(684, 0, 40, 40, m_USBTip);
    m_USBTip->setBackgroundBmpPath(QString(":/Images/StatusBarWidgetUSBTipDisable.png"));
    m_USBTip->setVisible(true);
    initializeBackButton();
    m_BackButton->setVisible(true);
}

void StatusBarWidgetPrivate::initializeDate()
{
    if (NULL == m_DateText) {
        Q_Q(StatusBarWidget);
        m_DateText = new TextWidget(q);
        m_DateText->setAlignmentFlag(Qt::AlignLeft | Qt::AlignVCenter);
        m_DateText->setLanguageType(TextWidget::T_NoTranslate);
        m_DateText->setText(QDate::currentDate().toString(QString("yyyy/MM/dd")));
        g_Widget->geometryFit(0, 0, 200, 40, m_DateText);
        m_DateText->setVisible(true);
    }
}

void StatusBarWidgetPrivate::initializeTitle()
{
    if (NULL == m_Title) {
        Q_Q(StatusBarWidget);
        m_Title = new TextWidget(q);
        m_Title->setAlignmentFlag(Qt::AlignLeft | Qt::AlignVCenter);
        g_Widget->geometryFit(80, 0, 208, 40, m_Title);
        m_Title->setVisible(false);
    }
}

void StatusBarWidgetPrivate::initializeTime()
{
    if (NULL == m_Time) {
        Q_Q(StatusBarWidget);
        m_Time = new TextWidget(q);
        m_Time->setLanguageType(TextWidget::T_NoTranslate);
        g_Widget->geometryFit(300, 0, 200, 40, m_Time);
    }
    QDateTime dateTime = QDateTime::currentDateTime();
    if (HCT_12HourClock == SettingPersistent::getHourClockType()) {
        m_Time->setText(dateTime.toString(QString("hh:mmA")));
    } else {
        m_Time->setText(dateTime.toString(QString("hh:mm")));
    }
}

void StatusBarWidgetPrivate::initializeHomeButton()
{
    Q_Q(StatusBarWidget);
    if (NULL == m_HomeButton) {
        m_HomeButton = new BmpButton(q);
        g_Widget->geometryFit(0, 0, 76, 52, m_HomeButton);
        m_HomeButton->setNormalBmpPath(QString(":/Images/StatusBarWidgetHomeButtonNormal.png"));
        m_HomeButton->setPressBmpPath(QString(":/Images/StatusBarWidgetHomeButtonPress.png"));
        Qt::ConnectionType type = static_cast<Qt::ConnectionType>(Qt::UniqueConnection | Qt::AutoConnection);
        QObject::connect(m_HomeButton, ARKSENDER(released()),
                         q,            ARKRECEIVER(onBmpButtonRelease()),
                         type);
    }
}

void StatusBarWidgetPrivate::initializeBackButton()
{
    Q_Q(StatusBarWidget);
    if (NULL == m_BackButton) {
        m_BackButton = new BmpButton(q);
        g_Widget->geometryFit(724, 0, 76, 52, m_BackButton);
        m_BackButton->setNormalBmpPath(QString(":/Images/StatusBarWidgetBackButtonNormal.png"));
        m_BackButton->setPressBmpPath(QString(":/Images/StatusBarWidgetBackButtonPress.png"));
        Qt::ConnectionType type = static_cast<Qt::ConnectionType>(Qt::UniqueConnection | Qt::AutoConnection);
        QObject::connect(m_BackButton, ARKSENDER(released()),
                         q,            ARKRECEIVER(onBmpButtonRelease()),
                         type);
    }
}
