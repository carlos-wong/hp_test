#include "MainWidget.h"
#include "UserInterfaceUtility.h"
#include "StatusBarWidget/StatusBarWidget.h"
#include "MessageBox.h"
#include "BusinessLogic/Widget.h"
#include "HomeWidget/HomeWidget.h"
#include "BusinessLogic/BusinessLogicUtility.h"
#include "DiskWidget/USBDiskWidget/USBDiskWidget.h"
#include "DiskWidget/SDDiskWidget/SDDiskWidget.h"
#include "DiskWidget/MusicWidget/MusicWidget.h"
#include "DiskWidget/ImageWidget/ImageWidget.h"
#include "DiskWidget/VideoWidget/VideoWidget.h"
#include "LinkWidget/LinkWidget.h"
#include "AUXWidget/AUXWidget.h"
#include "CameraWidget/CameraWidget.h"
#include "SteeringWidget/SteeringWidget.h"
#include "SettingWidget/SettingWidget.h"
#include "RadioWidget/RadioWidget.h"
#include "BluetoothWidget/BluetoothWidget.h"
#include "VolumeWidget/VolumeWidget.h"
#include "Utility.h"
#include "UserInterfaceUtility.h"
#include "BmpWidget.h"
#include "ark_api.h"
#include "RunnableThread.h"
#include <QEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QSocketNotifier>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/types.h>
#include <linux/netlink.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <QMap>

class MainWidgetPrivate
{
    Q_DISABLE_COPY(MainWidgetPrivate)
public:
    explicit MainWidgetPrivate(MainWidget *parent);
    ~MainWidgetPrivate();
    void onThemeChange(const int type);
    void initializeNetlink();
    void initializeBasicWidget();
    void initializeStatusBarWidget();
    void initializeHomeWidget();
    void initializeSDDiskWidget();
    void initializeUSBDiskWidget();
    void initializeMusicWidget();
    void initializeImageWidget();
    void initializeRadioWidget();
    void initializeBluetoothWidget();
    void initializeVideoWidget();
    void initializeLinkWidget();
    void initializeAUXWidget();
    void initializeCameraWidget();
    void initializeSteeringWidget();
    void initializeSettingWidget();
    void initializeVolumeWidget();
    void connectAllSlots();
    void recordChildVisible(QWidget* obj);
    void restoreChildVisible(QWidget* obj);
    QSocketNotifier* m_SocketNotifier;
    BmpWidget* m_Background;
    HomeWidget* m_HomeWidget;
    RadioWidget* m_RadioWidget;
    BluetoothWidget* m_BluetoothWidget;
    USBDiskWidget* m_USBDiskWidget;
    SDDiskWidget* m_SDDiskWidget;
    MusicWidget* m_MusicWidget;
    ImageWidget* m_ImageWidget;
    VideoWidget* m_VideoWidget;
    LinkWidget* m_LinkWidget;
    AUXWidget* m_AUXWidget;
    CameraWidget* m_CameraWidget;
    SteeringWidget* m_SteeringWidget;
    SettingWidget* m_SettingWidget;
    StatusBarWidget* m_StatusBarWidget;
    VolumeWidget* m_VolumeWidget;
    bool m_DisplayVisible;
    QMap<QWidget*, bool> m_ChildrenVisibleMap;
private:
    MainWidget* m_Parent;
};

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
    , m_Private(new MainWidgetPrivate(this))
{
}

MainWidget::~MainWidget()
{
}

void MainWidget::makeSureComplete()
{
    m_Private->m_RadioWidget->makeSureComplete();
}

void MainWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(0, 0, 800, 480, this);
}

void MainWidget::onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status)
{
    switch (destinationType) {
    case Widget::T_ViewPaper0: {
        if (WidgetStatus::RequestShow == status) {
            m_Private->initializeHomeWidget();
        } else if (WidgetStatus::Show == status) {
            m_Private->initializeSDDiskWidget();
            m_Private->initializeUSBDiskWidget();
            if (!m_Private->m_Background->isVisible()) {
                m_Private->m_Background->setVisible(true);
            }
        }
        break;
    }
    case Widget::T_ViewPaper1: {
        if (WidgetStatus::Show == status) {
            if (!m_Private->m_Background->isVisible()) {
                m_Private->m_Background->setVisible(true);
            }
            static bool flag(true);
            if (flag) {
                flag = false;
                g_Setting->synchronizeSteeringWheelStatus();
            }
        }
        break;
    }
    case Widget::T_Bluetooth:
    case Widget::T_BluetoothDial:
    case Widget::T_BluetoothContact:
    case Widget::T_BluetoothRecord:
    case Widget::T_BluetoothMusic:
    case Widget::T_BluetoothSetting: {
        if (WidgetStatus::RequestShow == status) {
            m_Private->initializeBluetoothWidget();
        } else if (WidgetStatus::Show == status) {
            if (!m_Private->m_Background->isVisible()) {
                m_Private->m_Background->setVisible(true);
            }
        }
        break;
    }
    case Widget::T_SDImage:
    case Widget::T_USBImage:
    case Widget::T_SDImageFullScreen:
    case Widget::T_USBImageFullScreen:
    case Widget::T_Carlife:
    case Widget::T_Auto:
    case Widget::T_Carplay:
    case Widget::T_SDVideo:
    case Widget::T_USBVideo:
    case Widget::T_SDVideoFullScreen:
    case Widget::T_USBVideoFullScreen: {
        if (WidgetStatus::Show == status) {
            if (m_Private->m_Background->isVisible()) {
                m_Private->m_Background->setVisible(false);
            }
        }
        break;
    }
    case Widget::T_AUX: {
        if (WidgetStatus::RequestShow == status) {
            m_Private->initializeAUXWidget();
        } else if (WidgetStatus::Show == status) {
            if (m_Private->m_Background->isVisible()) {
                m_Private->m_Background->setVisible(false);
            }
        }
        break;
    }
    case Widget::T_AUXFullScreen: {
        if (WidgetStatus::Show == status) {
            if (m_Private->m_Background->isVisible()) {
                m_Private->m_Background->setVisible(false);
            }
        }
        break;
    }
    case Widget::T_Camera: {
        if (WidgetStatus::RequestShow == status) {
            m_Private->initializeCameraWidget();
        } else if (WidgetStatus::Show == status) {
            if (m_Private->m_Background->isVisible()) {
                m_Private->m_Background->setVisible(false);
            }
        }
        break;
    }
    case Widget::T_CameraFullScreen: {
        if (WidgetStatus::Show == status) {
            if (m_Private->m_Background->isVisible()) {
                m_Private->m_Background->setVisible(false);
            }
        }
        break;
    }
    case Widget::T_SDDiskMusic:
    case Widget::T_SDDiskImage:
    case Widget::T_SDDiskVideo:
    case Widget::T_USBDiskMusic:
    case Widget::T_USBDiskImage:
    case Widget::T_USBDiskVideo: {
        if (WidgetStatus::Show == status) {
            if (!m_Private->m_Background->isVisible()) {
                m_Private->m_Background->setVisible(true);
            }
        }
        break;
    }
    case Widget::T_Link: {
        if (WidgetStatus::RequestShow == status) {
            m_Private->initializeLinkWidget();
        } else if (WidgetStatus::Show == status) {
            if (!m_Private->m_Background->isVisible()) {
                m_Private->m_Background->setVisible(true);
            }
        }
        break;
    }
    case Widget::T_Steering: {
        if (WidgetStatus::RequestShow == status) {
            m_Private->initializeSteeringWidget();
        } else if (WidgetStatus::Show == status) {
            if (!m_Private->m_Background->isVisible()) {
                m_Private->m_Background->setVisible(true);
            }
        }
        break;
    }
    case Widget::T_Setting: {
        if (WidgetStatus::RequestShow == status) {
            m_Private->initializeSettingWidget();
        } else if (WidgetStatus::Show == status) {
            m_Private->m_Background->setVisible(true);
        }
        break;
    }
    case Widget::T_SettingStyle: {
        if (WidgetStatus::Show == status) {
            connectSignalAndSlotByNamesake(g_Setting, this, ARKRECEIVER(onThemeChange(const int)));
            m_Private->m_Background->setVisible(true);
        }
        break;
    }
    default: {
        if (WidgetStatus::Show == status) {
            m_Private->m_Background->setVisible(true);
        }
        break;
    }
    }
}

void MainWidget::onThemeChange(const int type)
{
    m_Private->onThemeChange(type);
}

static void initializeRunnableCallback(void *paramater)
{
    system("insmod /lib/modules/3.4.0/kernel/drivers/usb/musb/musb_hdrc.ko");
    usleep(10);
    system("insmod /lib/modules/3.4.0/kernel/drivers/usb/musb/ark1680_musb.ko");
    usleep(10);
    if (QString(qgetenv("CARPLAY_USB_INDEX")).contains(QString("1"))) {
        system("insmod /lib/modules/3.4.0/kernel/drivers/usb/gadget/g_zero.ko");
        usleep(10);
        system("insmod /lib/modules/3.4.0/kernel/drivers/usb/gadget/g_ncm.ko");
        usleep(10);
    }else{
        system("insmod /lib/modules/3.4.0/kernel/drivers/usb/gadget/g_ncm.ko");
        usleep(10);
        system("insmod /lib/modules/3.4.0/kernel/drivers/usb/gadget/g_zero.ko");
        usleep(10);  
    }

    system("insmod /lib/modules/3.4.0/kernel/drivers/ark/sdmmc/ark_dw_mmc.ko");
    usleep(10);
    system("echo otg > /sys/devices/platform/musb-ark1680.0/musb-hdrc.0/mode");
    usleep(10);
    system("echo otg > /sys/devices/platform/musb-ark1680.1/musb-hdrc.1/mode");
    usleep(10);
    if (QString(qgetenv("PROTOCOL_ID")).contains(QString("yaoxi"))) {
        usleep(10);
        system("insmod /lib/modules/3.4.0/kernel/drivers/usb/gadget/g_webcam.ko");
        usleep(10);
        system("echo otg > /sys/devices/platform/musb-ark1680.1/musb-hdrc.1/mode");
    }

    memallocload();
    driverload();
}

void MainWidget::onStartComplete()
{
    g_Radio->initialize();

    m_Private->initializeRadioWidget();
    // m_Private->initializeSDDiskWidget();
    // m_Private->initializeUSBDiskWidget();
    m_Private->initializeVolumeWidget();
    m_Private->initializeBluetoothWidget();
    m_Private->initializeNetlink();
    connectSignalAndSlotByNamesake(g_Widget, this, ARKRECEIVER(onWidgetTypeChange(const Widget::Type, const Widget::Type, const QString &)));

#ifndef DESKTOP_AMD64
    struct ArkControl ctrl;
    ctrl.type = ACLT_Req_bt_status;
    struct ArkProtocol bt_status;
    bt_status.type = AT_Control;
    bt_status.data = (void*)&ctrl;
    bt_status.prio = AP_PRIO_NORMAL;
    ark_protocol_send(&bt_status);

    struct ArkControl ctrl2;
    ctrl2.type = ACLT_Req_bt_status;
    struct ArkProtocol bt_status2;
    bt_status2.type = AT_Control;
    bt_status2.data = (void*)&ctrl;
    bt_status2.prio = AP_PRIO_NORMAL;
    ark_protocol_send(&bt_status2);
#endif

    m_Private->initializeLinkWidget();
    qDebug() << "++++++++onStartComplete+++++++++++++++!";

#ifdef DESKTOP_AMD64
    m_Private->initializeMusicWidget();
    m_Private->initializeVideoWidget();
    m_Private->initializeImageWidget();
    m_Private->initializeSDDiskWidget();
    m_Private->initializeUSBDiskWidget();
#endif
}

void MainWidget::onActivated()
{
    QByteArray data;
    data.resize(1024);
    size_t len = ::read(m_Private->m_SocketNotifier->socket(), data.data(), 1024);
    data.resize(len);
    data = data.replace(0, '\n').trimmed();
    QString filter(data);
    if (filter.contains(QString("SUBSYSTEM=module"))
            || filter.contains(QString("SUBSYSTEM=mmc_host"))
            || filter.contains(QString("SUBSYSTEM=drivers"))
            || filter.contains(QString("SUBSYSTEM=platform"))
            || filter.contains(QString("SUBSYSTEM=sound"))
            || filter.contains(QString("SUBSYSTEM=udc"))
            || filter.contains(QString("SUBSYSTEM=net"))
            || filter.contains(QString("SUBSYSTEM=usb"))
            || filter.contains(QString("SUBSYSTEM=usb_device"))
            || filter.contains(QString("SUBSYSTEM=misc"))
            || filter.contains(QString("SUBSYSTEM=queues"))
            || filter.contains(QString("SUBSYSTEM=video4linux"))) {
        if (!filter.contains(QString("PRODUCT=5ac/"))) {
            return;
        }
    }
    if (filter.contains(QString("DEVNAME=sd"))
            || filter.contains(QString("DEVNAME=mmcblk"))) {
        m_Private->initializeSDDiskWidget();
        m_Private->initializeUSBDiskWidget();
        m_Private->initializeMusicWidget();
        m_Private->initializeImageWidget();
        m_Private->initializeVideoWidget();
        g_Multimedia->startMultimedia();
        m_Private->m_SocketNotifier->setEnabled(false);
        ::close(m_Private->m_SocketNotifier->socket());
    }
}

void MainWidget::onReveringWidgetVisibleChanged(bool visible)
{
    if (visible) {
        m_Private->recordChildVisible(m_Private->m_Background);
        m_Private->recordChildVisible(m_Private->m_StatusBarWidget);
        m_Private->recordChildVisible(m_Private->m_HomeWidget);
        m_Private->recordChildVisible(m_Private->m_RadioWidget);
        m_Private->recordChildVisible(m_Private->m_BluetoothWidget);
        m_Private->recordChildVisible(m_Private->m_USBDiskWidget);
        m_Private->recordChildVisible(m_Private->m_SDDiskWidget);
        m_Private->recordChildVisible(m_Private->m_MusicWidget);
        m_Private->recordChildVisible(m_Private->m_ImageWidget);
        m_Private->recordChildVisible(m_Private->m_VideoWidget);
//        m_Private->recordChildVisible(m_Private->m_LinkWidget);
        m_Private->recordChildVisible(m_Private->m_AUXWidget);
        m_Private->recordChildVisible(m_Private->m_CameraWidget);
        m_Private->recordChildVisible(m_Private->m_SteeringWidget);
        m_Private->recordChildVisible(m_Private->m_SettingWidget);
    }
    else {
        m_Private->restoreChildVisible(m_Private->m_Background);
        m_Private->restoreChildVisible(m_Private->m_StatusBarWidget);
        m_Private->restoreChildVisible(m_Private->m_HomeWidget);
        m_Private->restoreChildVisible(m_Private->m_RadioWidget);
        m_Private->restoreChildVisible(m_Private->m_BluetoothWidget);
        m_Private->restoreChildVisible(m_Private->m_USBDiskWidget);
        m_Private->restoreChildVisible(m_Private->m_SDDiskWidget);
        m_Private->restoreChildVisible(m_Private->m_MusicWidget);
        m_Private->restoreChildVisible(m_Private->m_ImageWidget);
        m_Private->restoreChildVisible(m_Private->m_VideoWidget);
//        m_Private->restoreChildVisible(m_Private->m_LinkWidget);
        m_Private->restoreChildVisible(m_Private->m_AUXWidget);
        m_Private->restoreChildVisible(m_Private->m_CameraWidget);
        m_Private->restoreChildVisible(m_Private->m_SteeringWidget);
        m_Private->restoreChildVisible(m_Private->m_SettingWidget);
    }
}

MainWidgetPrivate::MainWidgetPrivate(MainWidget *parent)
    : m_Parent(parent)
{
    m_SocketNotifier = NULL;
    m_Background = NULL;
    m_HomeWidget = NULL;
    m_RadioWidget = NULL;
    m_BluetoothWidget = NULL;
    m_USBDiskWidget = NULL;
    m_SDDiskWidget = NULL;
    m_MusicWidget = NULL;
    m_ImageWidget = NULL;
    m_VideoWidget = NULL;
    m_LinkWidget = NULL;
    m_AUXWidget = NULL;
    m_CameraWidget = NULL;
    m_SteeringWidget = NULL;
    m_SettingWidget = NULL;
    m_StatusBarWidget = NULL;
    m_VolumeWidget = NULL;
    m_DisplayVisible = false;
    initializeBasicWidget();
    connectAllSlots();
}

MainWidgetPrivate::~MainWidgetPrivate()
{
}

void MainWidgetPrivate::onThemeChange(const int type)
{
    switch (type) {
    case TT_ThemeOne: {
        m_Background->setBackgroundBmpPath(QString(":/Images/hw_01_theme_main_01.jpg"));
        break;
    }
    case TT_ThemeTwo: {
        m_Background->setBackgroundBmpPath(QString(":/Images/hw_01_theme_main_02.png"));
        break;
    }
    case TT_ThemeThree: {
        m_Background->setBackgroundBmpPath(QString(":/Images/hw_01_theme_main_03.jpg"));
        break;
    }
    case TT_ThemeFour: {
        m_Background->setBackgroundBmpPath(QString(":/Images/hw_01_theme_main_04.png"));
        break;
    }
    case TT_ThemeFive: {
        m_Background->setBackgroundBmpPath(QString(":/Images/hw_01_theme_main_05.png"));
        break;
    }
    case TT_ThemeSix: {
        m_Background->setBackgroundBmpPath(QString(":/Images/hw_01_theme_main_06.png"));
        break;
    }
    default: {
        break;
    }
    }
}

void MainWidgetPrivate::initializeNetlink()
{
    if (NULL == m_SocketNotifier) {
        int socket = ::socket(PF_NETLINK, SOCK_RAW, NETLINK_KOBJECT_UEVENT);
        if (-1 == socket) {
            perror("socket");
            qDebug() << "create netlink socket fail!";
            return;
        }
        struct sockaddr_nl sock_nl;
        memset(&sock_nl, 0, sizeof(struct sockaddr_nl));
        sock_nl.nl_family = AF_NETLINK;
        sock_nl.nl_groups = 1;
        int bind = ::bind(socket, (struct sockaddr*)&sock_nl, sizeof(struct sockaddr_nl));
        if (-1 == bind) {
            perror("bind");
            qDebug() << "bind netlink socket fail!";
            return;
        }
        m_SocketNotifier = new QSocketNotifier(socket, QSocketNotifier::Read, m_Parent);
        QObject::connect(m_SocketNotifier, ARKSENDER(activated(int)),
                         m_Parent,         ARKRECEIVER(onActivated()));
    }
}

void MainWidgetPrivate::initializeBasicWidget()
{
    m_Background = new BmpWidget(m_Parent);
    onThemeChange(SettingPersistent::getThemeType());
    g_Widget->geometryFit(0, 0, 800, 480, m_Background);
    m_Background->setVisible(true);
//    initializeRadioWidget();
    initializeStatusBarWidget();
    initializeHomeWidget();

    CustomRunnable* runnable = new CustomRunnable();
    runnable->setCallbackFunction(initializeRunnableCallback, NULL);
    QThreadPool::globalInstance()->start(runnable, 1);

    m_Parent->setVisible(true);
}

void MainWidgetPrivate::initializeStatusBarWidget()
{
    if (NULL == m_StatusBarWidget) {
        m_StatusBarWidget = new StatusBarWidget(m_Parent);
    }
}

void MainWidgetPrivate::initializeHomeWidget()
{
    if (NULL == m_HomeWidget) {
        m_HomeWidget = new HomeWidget(m_Parent);
        g_Widget->setWidgetType(Widget::T_ViewPaper0, Widget::T_Radio, WidgetStatus::RequestShow);
    }
}

void MainWidgetPrivate::initializeSDDiskWidget()
{
    if (NULL == m_SDDiskWidget) {
        m_SDDiskWidget = new SDDiskWidget(m_Parent);
    }
}

void MainWidgetPrivate::initializeUSBDiskWidget()
{
    if (NULL == m_USBDiskWidget) {
        m_USBDiskWidget = new USBDiskWidget(m_Parent);
    }
}

void MainWidgetPrivate::initializeMusicWidget()
{
    if (NULL == m_MusicWidget) {
        m_MusicWidget = new MusicWidget(m_Parent);
    }
}

void MainWidgetPrivate::initializeImageWidget()
{
    if (NULL == m_ImageWidget) {
        m_ImageWidget = new ImageWidget(m_Parent);
    }
}

void MainWidgetPrivate::initializeRadioWidget()
{
    if (NULL == m_RadioWidget) {
        m_RadioWidget = new RadioWidget(m_Parent);
    }
}

void MainWidgetPrivate::initializeBluetoothWidget()
{
    if (NULL == m_BluetoothWidget) {
        m_BluetoothWidget = new BluetoothWidget(m_Parent);
    }
}

void MainWidgetPrivate::initializeVideoWidget()
{
    if (NULL == m_VideoWidget) {
        m_VideoWidget = new VideoWidget(m_Parent);
    }
}

void MainWidgetPrivate::initializeLinkWidget()
{
    if (NULL == m_LinkWidget) {
        m_LinkWidget = new LinkWidget(m_Parent);
//        g_Widget->setWidgetType(Widget::T_Link, Widget::T_ViewPaper1, WidgetStatus::RequestShow);
    }
}

void MainWidgetPrivate::initializeAUXWidget()
{
    if (NULL == m_AUXWidget) {
        m_AUXWidget = new AUXWidget(m_Parent);
        g_Widget->setWidgetType(Widget::T_AUX, Widget::T_ViewPaper1, WidgetStatus::RequestShow);
    }
}

void MainWidgetPrivate::initializeCameraWidget()
{
    if (NULL == m_CameraWidget) {
        m_CameraWidget = new CameraWidget(m_Parent);
        g_Widget->setWidgetType(Widget::T_Camera, Widget::T_ViewPaper1, WidgetStatus::RequestShow);
    }
}

void MainWidgetPrivate::initializeSteeringWidget()
{
    if (NULL == m_SteeringWidget) {
        m_SteeringWidget = new SteeringWidget(m_Parent);
        g_Widget->setWidgetType(Widget::T_Steering, Widget::T_ViewPaper1, WidgetStatus::RequestShow);
    }
}

void MainWidgetPrivate::initializeSettingWidget()
{
    if (NULL == m_SettingWidget) {
        m_SettingWidget = new SettingWidget(m_Parent);
        g_Widget->setWidgetType(Widget::T_Setting, Widget::T_ViewPaper1, WidgetStatus::RequestShow);
    }
}

void MainWidgetPrivate::initializeVolumeWidget()
{
    if (NULL == m_VolumeWidget) {
        m_VolumeWidget = new VolumeWidget(m_Parent);
    }
}

void MainWidgetPrivate::connectAllSlots()
{
    QObject::connect(g_Setting, ARKSENDER(onStartComplete()),
                     m_Parent,  ARKRECEIVER(onStartComplete()));
}

void MainWidgetPrivate::recordChildVisible(QWidget *obj)
{
    if (NULL != obj) {
        m_ChildrenVisibleMap[obj] = obj->isVisible();
        obj->hide();
    }
}

void MainWidgetPrivate::restoreChildVisible(QWidget *obj)
{
    if (NULL != obj) {
        obj->setVisible(m_ChildrenVisibleMap.value(obj));
    }
}
