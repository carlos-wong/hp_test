#include "Launcher.h"
#include "CalibrateWidget/CalibrateWidget.h"
#include "MainWidget/MainWidget.h"
#include "ReversingWidget/ReversingWidget.h"
#include "IdleWidget/IdleWidget.h"
#include "Utility.h"
#include "AutoConnect.h"
#include "ArkApplication.h"
#include "UserInterfaceUtility.h"
#include "BusinessLogic/Setting.h"
#include "RunnableThread.h"
#include "BusinessLogic/Bluetooth.h"
#include "BusinessLogic/Multimedia.h"
#include "BusinessLogic/Audio.h"
#include "BusinessLogic/Radio.h"
#include "BusinessLogic/Setting.h"
#include "BusinessLogic/Link.h"
#include "ark_api.h"
#include <QDebug>
#include <QApplication>
#include <QThread>

class LauncherPrivate {
public:
    explicit LauncherPrivate(Launcher* parent);
    ~LauncherPrivate();
    void initializeParent();
    void initializeLanguage();
    void initializeIdleWidget();
    void initializeReversingWidget();
    void initializeCalibrateWidget();
    void onArkCarHandler(const ArkProtocolWrapper &protocol);
    void onArkKeyHandler(const ArkProtocolWrapper &protocol);
private:
    Q_DECLARE_PUBLIC(Launcher)
    Launcher* const q_ptr;
    MainWidget* m_MainWidget;
    CalibrateWidget* m_CalibrateWidget;
    ReversingWidget* m_ReversingWidget;
    IdleWidget* m_IdleWidget;
};

Launcher::Launcher(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new LauncherPrivate(this))
{
}

Launcher::~Launcher()
{
    if (NULL != d_ptr) {
        delete d_ptr;
    }
}

void Launcher::timerEvent(QTimerEvent *event)
{
    ArkApp->startTimer(60);
    // SettingPersistent::restoreBrightness();
    // SettingPersistent::restoreContrast();
    // SettingPersistent::restoreSaturation();
    g_Setting->setStartComplete();
    g_Setting->setLauncherTVOUT(true);
    killTimer(event->timerId());
}

void Launcher::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(0, 0, 800, 480, this);
}

void Launcher::onArkProtocol(const ArkProtocolWrapper &protocol)
{
    Q_D(Launcher);
    switch (protocol.type) {
    case AT_Car: {
        d->onArkCarHandler(protocol);
        break;
    }
    case AT_Key: {
        d->onArkKeyHandler(protocol);
        break;
    }
    default: {
        break;
    }
    }
}

void Launcher::onStartComplete()
{
    Q_D(Launcher);
#ifndef DESKTOP_AMD64
    // d->m_MainWidget->makeSureComplete();
#endif
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setVisible(true);
    qApp->flush();
    qApp->sendPostedEvents();
    startTimer(0);
}

LauncherPrivate::LauncherPrivate(Launcher* parent)
    : q_ptr(parent)
{
    m_MainWidget = NULL;
    m_CalibrateWidget = NULL;
    m_ReversingWidget = NULL;
    m_IdleWidget = NULL;
    initializeLanguage();
    initializeParent();
}

LauncherPrivate::~LauncherPrivate()
{
}

void LauncherPrivate::initializeParent()
{
    Q_Q(Launcher);
    g_Radio;
    g_Bluetooth;
    QObject::connect(g_Radio, ARKSENDER(startComplete()),
                     q,       ARKRECEIVER(onStartComplete()),
                     Qt::QueuedConnection);
    g_Setting->initializeArkProtocol();
    connectSignalAndSlotByNamesake(g_Setting, q, ARKRECEIVER(onArkProtocol(const ArkProtocolWrapper&)));
    m_MainWidget = new MainWidget(q);

#ifndef DESKTOP_AMD64
    if (qgetenv("QWS_ARK_MT_DEVICE").isEmpty()) {
        if (!SettingPersistent::getCalibrate()) {
            initializeCalibrateWidget();
            m_CalibrateWidget->startCalibrate();
        }
    }
    q->onStartComplete();
#else
    q->onStartComplete();   
#endif
}

void LauncherPrivate::initializeLanguage()
{
    QString languagePath;
    languagePath.clear();
    const int language = SettingPersistent::getLanguage();
    switch (language) {
    case LT_Chinese: {
        languagePath = QString(":/Languages/Launcher_CN.qm");
        break;
    }
    case LT_Japaness: {
        languagePath = QString(":/Languages/Launcher_JP.qm");
        break;
    }
    case LT_Korean: {
        languagePath = QString(":/Languages/Launcher_KO.qm");
        break;
    }
    case LT_Russian: {
        languagePath = QString(":/Languages/Launcher_RU.qm");
        break;
    }
    case LT_English:
    default: {
        languagePath = QString(":/Languages/Launcher_EN.qm");
        break;
    }
    }
    if (!ArkApp->installTranslatorPath(languagePath)) {
        qDebug() << "QTranslator load" << language << languagePath << "fail!";
    }
}

void LauncherPrivate::initializeIdleWidget()
{
    if (NULL == m_IdleWidget) {
        Q_Q(Launcher);
        m_IdleWidget = new IdleWidget(q);
    }
}

void LauncherPrivate::initializeReversingWidget()
{
    if (NULL == m_ReversingWidget) {
        Q_Q(Launcher);
        m_ReversingWidget = new ReversingWidget(q);
        Qt::ConnectionType type = static_cast<Qt::ConnectionType>(Qt::UniqueConnection | Qt::AutoConnection);
        QObject::connect(m_ReversingWidget, ARKSENDER(visibleChanged(bool)),
                         m_MainWidget,            ARKRECEIVER(onReveringWidgetVisibleChanged(bool)),
                         type);
    }
}

void LauncherPrivate::initializeCalibrateWidget()
{
    if (NULL == m_CalibrateWidget) {
        Q_Q(Launcher);
        m_CalibrateWidget = new CalibrateWidget(q);
    }
}

void LauncherPrivate::onArkCarHandler(const ArkProtocolWrapper &protocol)
{
    struct ArkCar* car = arkProtocolWrapperToArkCar(protocol);
    if (NULL != car) {
        switch (car->type) {
        case ACT_ACC: {
            if (ACS_ACCOff == car->status) {
                if (DST_On == g_Setting->getDisplayScreenType()) {
                    g_Setting->setDisplayScreen(DST_Off);
                }
                g_Audio->requestSpeaker(SSI_PowerOff);
                struct ArkControl control;
                memset((void*)&control, 0, sizeof(struct ArkControl));
                struct ArkProtocol protocol;
                memset((void*)&protocol, 0, sizeof(struct ArkProtocol));
                protocol.type = AT_Control;
                protocol.direction = AD_Setter;
                protocol.data = &control;
                control.type = ACLT_RequestSourceSwitch;
                unsigned char data[1] = {0x00};
                control.data = data;
                control.length = 1;
                g_Setting->setArkProtocol(ArkProtocolWrapper(protocol));
                qApp->exit();
            }
            break;
        }
        case ACT_Reversing: {
            if (ACS_ReversingOn == car->status) {
                initializeReversingWidget();
            }
            switch (g_Widget->currentWidgetType()) {
            case Widget::T_AUX:
            case Widget::T_AUXFullScreen:
            case Widget::T_Camera:
            case Widget::T_CameraFullScreen: {
                if (ACS_ReversingOn == car->status) {
                    g_Setting->setLauncherTVOUT(false);
                }
                break;
            }
            case Widget::T_SDVideo:
            case Widget::T_SDVideoFullScreen:
            case Widget::T_USBVideo:
            case Widget::T_USBVideoFullScreen:
            case Widget::T_Carlife:
            case Widget::T_Auto:
            case Widget::T_Carplay: {
                if (ACS_ReversingOn == car->status) {
                    QFile file(QString("/tmp/video"));
                    if (!file.exists()
                            && file.open(QIODevice::WriteOnly)) {
                        file.close();
                    }
                    g_Setting->setLauncherTVOUT(false);
                } else if (ACS_ReversingOff == car->status) {
                    if (QFile::exists(QString("/tmp/video"))) {
                        QFile::remove(QString("/tmp/video"));
                    }
                }
                break;
            }
            default: {
                g_Setting->setLauncherTVOUT(ACS_ReversingOff == car->status);
                break;
            }
            }
            if (NULL != m_ReversingWidget) {
                m_ReversingWidget->onArkCarHandler(car);
            }
            break;
        }
        default: {
            break;
        }
        }
    }
}

static bool handlebluetoothmusicwidget()
{
    bool flag = false;
    if (BCS_Connected == g_Bluetooth->connectStatus()) {
        flag = AS_Radio == g_Audio->getAudioSource();
    }
    if (flag) {
        g_Widget->setWidgetType(Widget::T_BluetoothMusic, Widget::T_Undefine, WidgetStatus::RequestShow);
    }
    return flag;
}

static bool handlesdwidget()
{
    bool flag = false;
    bool empty = g_Multimedia->musicListEmpty(DWT_SDDisk)
            && g_Multimedia->imageListEmpty(DWT_SDDisk)
            && g_Multimedia->videoListEmpty(DWT_SDDisk);
    qDebug() << __PRETTY_FUNCTION__ << __LINE__ << !empty << g_Multimedia->getDeviceStatus(DWT_SDDisk);
    if ((DWS_Empty != g_Multimedia->getDeviceStatus(DWT_SDDisk))
            && (DWS_Remove != g_Multimedia->getDeviceStatus(DWT_SDDisk))
            && (DWS_Unsupport != g_Multimedia->getDeviceStatus(DWT_SDDisk))
            && (!empty)) {
        qDebug() << __PRETTY_FUNCTION__ << __LINE__;
        if (DWT_SDDisk == g_Multimedia->getDeviceActiveType()) {
            qDebug() << __PRETTY_FUNCTION__ << __LINE__;
            if (Multimedia::F_ImageUnfocus == g_Multimedia->getImageFocus()) {
                qDebug() << __PRETTY_FUNCTION__ << __LINE__;
                flag = AS_Radio == g_Audio->getAudioSource();
                if (!flag) {
                    if (BCS_Connected == g_Bluetooth->connectStatus()) {
                        qDebug() << __PRETTY_FUNCTION__ << __LINE__;
                        flag = AS_BluetoothMusic == g_Audio->getAudioSource();
                    }
                }
            }
        } else if (DWT_USBDisk == g_Multimedia->getDeviceActiveType()) {
            qDebug() << __PRETTY_FUNCTION__ << __LINE__;
            if (Multimedia::F_ImageUnfocus == g_Multimedia->getImageFocus()) {
                flag = AS_Radio == g_Audio->getAudioSource();
                if (!flag) {
                    flag = AS_BluetoothMusic == g_Audio->getAudioSource();
                }
            }
        } else if (BCS_Connected == g_Bluetooth->connectStatus()) {
            qDebug() << __PRETTY_FUNCTION__ << __LINE__;
            flag = AS_BluetoothMusic == g_Audio->getAudioSource();
        } else if ((BCS_Idle == g_Bluetooth->connectStatus())
                   || (BCS_Connecting == g_Bluetooth->connectStatus())) {
            qDebug() << __PRETTY_FUNCTION__ << __LINE__;
            if (DWT_Undefine == g_Multimedia->getDeviceActiveType()) {
                qDebug() << __PRETTY_FUNCTION__ << __LINE__;
                flag = AS_Radio == g_Audio->getAudioSource();
            }
        }
        if (flag) {
            g_Widget->setWidgetType(Widget::T_SDDisk, Widget::T_Undefine, WidgetStatus::RequestShow);
        }
    }
    return flag;
}

static bool handleusbwidget()
{
    bool flag = false;
    qDebug() << __PRETTY_FUNCTION__ << __LINE__;
    bool empty = g_Multimedia->musicListEmpty(DWT_USBDisk)
            && g_Multimedia->imageListEmpty(DWT_USBDisk)
            && g_Multimedia->videoListEmpty(DWT_USBDisk);
    if ((DWS_Empty != g_Multimedia->getDeviceStatus(DWT_USBDisk))
            && (DWS_Remove != g_Multimedia->getDeviceStatus(DWT_USBDisk))
            && (DWS_Unsupport != g_Multimedia->getDeviceStatus(DWT_USBDisk))
            && (!empty)) {
        qDebug() << __PRETTY_FUNCTION__ << __LINE__;
        if ((DWS_Empty != g_Multimedia->getDeviceStatus(DWT_SDDisk))
                && (DWS_Remove != g_Multimedia->getDeviceStatus(DWT_SDDisk))
                && (DWS_Unsupport != g_Multimedia->getDeviceStatus(DWT_SDDisk))) {
            qDebug() << __PRETTY_FUNCTION__ << __LINE__;
            qDebug() << g_Multimedia->getDeviceActiveType();
            flag = DWT_SDDisk == g_Multimedia->getDeviceActiveType();
        } else if (DWT_USBDisk == g_Multimedia->getDeviceActiveType()) {
            qDebug() << __PRETTY_FUNCTION__ << __LINE__;
            if (Multimedia::F_ImageUnfocus == g_Multimedia->getImageFocus()) {
                qDebug() << __PRETTY_FUNCTION__ << __LINE__;
                flag = AS_Radio == g_Audio->getAudioSource();
                if (!flag) {
                    if (BCS_Connected == g_Bluetooth->connectStatus()) {
                        qDebug() << __PRETTY_FUNCTION__ << __LINE__;
                        flag = AS_BluetoothMusic == g_Audio->getAudioSource();
                    }
                }
            }
        } else if (BCS_Connected == g_Bluetooth->connectStatus()) {
            qDebug() << __PRETTY_FUNCTION__ << __LINE__;
            flag = AS_BluetoothMusic == g_Audio->getAudioSource();
        } else if ((BCS_Idle == g_Bluetooth->connectStatus())
                   || (BCS_Connecting == g_Bluetooth->connectStatus())) {
            qDebug() << __PRETTY_FUNCTION__ << __LINE__;
            qDebug() << g_Multimedia->getDeviceActiveType();
            if (DWT_Undefine == g_Multimedia->getDeviceActiveType()) {
                qDebug() << __PRETTY_FUNCTION__ << __LINE__;
                flag = AS_Radio == g_Audio->getAudioSource();
            }
        }
        if (flag) {
            g_Widget->setWidgetType(Widget::T_USBDisk, Widget::T_Undefine, WidgetStatus::RequestShow);
        }
    }
    return flag;
}

static bool handleradiowidget()
{
    bool flag = false;
    bool usbempty = g_Multimedia->musicListEmpty(DWT_USBDisk)
            && g_Multimedia->imageListEmpty(DWT_USBDisk)
            && g_Multimedia->videoListEmpty(DWT_USBDisk);
    bool sdempty = g_Multimedia->musicListEmpty(DWT_SDDisk)
            && g_Multimedia->imageListEmpty(DWT_SDDisk)
            && g_Multimedia->videoListEmpty(DWT_SDDisk);
    if ((DWS_Empty != g_Multimedia->getDeviceStatus(DWT_USBDisk))
            && (DWS_Remove != g_Multimedia->getDeviceStatus(DWT_USBDisk))
            && (DWS_Unsupport != g_Multimedia->getDeviceStatus(DWT_USBDisk))
            && (!usbempty)) {
        flag = DWT_USBDisk == g_Multimedia->getDeviceActiveType();
    } else if ((DWS_Empty != g_Multimedia->getDeviceStatus(DWT_SDDisk))
               && (DWS_Remove != g_Multimedia->getDeviceStatus(DWT_SDDisk))
               && (DWS_Unsupport != g_Multimedia->getDeviceStatus(DWT_SDDisk))
               && (!sdempty)) {
        flag = DWT_SDDisk == g_Multimedia->getDeviceActiveType();
    } else if (BCS_Connected == g_Bluetooth->connectStatus()) {
        flag = AS_BluetoothMusic == g_Audio->getAudioSource();
    }
    if (flag) {
        g_Widget->setWidgetType(Widget::T_Radio, Widget::T_Undefine, WidgetStatus::RequestShow);
    }
    return flag;
}

void LauncherPrivate::onArkKeyHandler(const ArkProtocolWrapper &protocol)
{
    struct ArkKey* key = arkProtocolWrapperToArkKey(protocol);
    if ((NULL != key)
            && (!protocol.handler)) {
        bool filter(false);
        filter = (AKS_Press == key->status) || filter;
        filter = (AKS_Release == key->status) || filter;
        filter = (AKS_Active == key->status) || filter;
        if (filter
                && (NULL != m_MainWidget)) {
            if (ACS_ReversingOn == g_Setting->getReversing()) {
                return ;
            }
            switch (g_Bluetooth->connectStatus()) {
            case BCS_Outgoing:
            case BCS_Incoming:
            case BCS_Talking: {
                if (!protocol.handler) {
                    ArkProtocolWrapperHandler(true);
                    return;
                }
                break;
            }
            default: {
                break;
            }
            }
            if (AKS_Press == key->status) {
                if (DST_Off == g_Setting->getDisplayScreenType()) {
                    if (!protocol.handler) {
                        ArkProtocolWrapperHandler(true);
                    }
                }
            } else if (AKS_Release == key->status) {
                if (DST_Off == g_Setting->getDisplayScreenType()) {
                    if (!protocol.handler) {
                        ArkProtocolWrapperHandler(true);
                        g_Setting->setDisplayScreen(DST_On);
                    }
                    return;
                }
                switch (key->type) {
                case AKT_CommonPower: {
                    initializeIdleWidget();
                    m_IdleWidget->onArkKeyHandler(key);
                    break;
                }
                case AKT_CommonHome: {
                    g_Widget->setWidgetType(Widget::T_Home, Widget::T_Undefine, WidgetStatus::RequestShow);
                    break;
                }
                case AKT_CommonReturn: {
                    g_Widget->setWidgetType(Widget::T_Back, Widget::T_Undefine, WidgetStatus::RequestShow);
                    break;
                }
                case AKT_CommonMode: {
                    if (!handlebluetoothmusicwidget()) {
                        if (!handlesdwidget()) {
                            if (!handleusbwidget()) {
                                if (!handleradiowidget()) {
                                    qDebug() << "unhandle mode key";
                                }
                            }
                        }
                    }
                    break;
                }
                case AKT_SwitchToBt: {
                    if ((BCS_Idle == g_Bluetooth->connectStatus())
                            || (BCS_Connecting == g_Bluetooth->connectStatus())) {
                        g_Widget->setWidgetType(Widget::T_BluetoothSetting, Widget::T_Undefine, WidgetStatus::RequestShow);
                    } else {
                        g_Widget->setWidgetType(Widget::T_BluetoothDial, Widget::T_Undefine, WidgetStatus::RequestShow);
                    }
                    break;
                }
                default: {
                    break;
                }
                }
            }
        }
    }
}
