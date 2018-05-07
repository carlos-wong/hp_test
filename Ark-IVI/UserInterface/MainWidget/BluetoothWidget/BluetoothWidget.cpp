#include "BluetoothWidget.h"
#include "AutoConnect.h"
#include "BluetoothDialWidget/BluetoothDialWidget.h"
#include "BluetoothContactWidget/BluetoothContactWidget.h"
#include "BluetoothMusicWidget/BluetoothMusicWidget.h"
#include "BluetoothRecordWidget/BluetoothRecordWidget.h"
#include "BluetoothSettingWidget/BluetoothSettingWidget.h"
#include "BluetoothToolWidget/BluetoothToolWidget.h"
#include "BusinessLogic/Link.h"
#include "MessageBox.h"
#include "TextWidget.h"
#include "BmpButton.h"
#include <QDebug>

namespace SourceString {
static const QString Device(QObject::tr("Device:"));
static const QString Pin_Code(QObject::tr("PinCode:"));
static const QString Turn_Off_Bluetooth_When_Connect_Carplay(QObject::tr("Turn Off Bluetooth\n"
                                                                         "When Connect Carplay!"));
static const QString Use_Carlife_Device_Connect_Local_Bluetooth_Device(QObject::tr("Use Carlife Device\n"
                                                                                   "Connect Local Bluetooth Device!"));
static const QString Switch_Carlife_Widget_To_Use_Bluetooth_Function(QObject::tr("Switch Carlife Widget\n"
                                                                                 "To Use Bluetooth Function!"));
static const QString Use_Auto_Device_Connect_Local_Bluetooth_Device(QObject::tr("Use Auto Device\n"
                                                                                "Connect Local Bluetooth Device!"));
static const QString Switch_Auto_Widget_To_Use_Bluetooth_Function(QObject::tr("Switch Auto Widget\n"
                                                                              "To Use Bluetooth Function!"));
}

class BluetoothWidgetPrivate
{
public:
    explicit BluetoothWidgetPrivate(BluetoothWidget* parent);
    ~BluetoothWidgetPrivate();
    void initializeParent();
    void connectAllSlots();
    void initializeMessageBox();
    void initializeDevice();
    void initializePin();
    void initializeDisconnectBtn();
    void initializeBluetoothDialWidget();
    void initializeBluetoothContactWidget();
    void initializeBluetoothRecordWidget();
    void initializeBluetoothMusicWidget();
    void initializeBluetoothSettingWidget();
    void initializeBluetoothToolWidget();
    void autoPaireHandler();
    void initializeTimer();
private:
    Q_DECLARE_PUBLIC(BluetoothWidget)
    BluetoothWidget* const q_ptr;
    MessageBox* m_MessageBox;
    BluetoothDialWidget* m_BluetoothDialWidget;
    BluetoothContactWidget* m_BluetoothContactWidget;
    BluetoothRecordWidget* m_BluetoothRecordWidget;
    BluetoothMusicWidget* m_BluetoothMusicWidget;
    BluetoothSettingWidget* m_BluetoothSettingWidget;
    BluetoothToolWidget* m_BluetoothToolWidget;
    bool m_RestoreBluetooth;
    TextWidget* m_Device;
    TextWidget* m_PinCode;
    BmpButton* m_DisconnectBtn;
    Widget::Type m_Type;
    QTimer* m_Timer;
};

BluetoothWidget::BluetoothWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new BluetoothWidgetPrivate(this))
{
}

BluetoothWidget::~BluetoothWidget()
{
    if (NULL != d_ptr) {
        delete d_ptr;
    }
}

void BluetoothWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(0, 0, 800, 480, this);
}

void BluetoothWidget::onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status)
{
    Q_D(BluetoothWidget);
    switch (destinationType) {
    case Widget::T_Home:
    case Widget::T_Back: {
        if ((BCS_Connected == g_Bluetooth->connectStatus())
                || (BCS_Idle == g_Bluetooth->connectStatus())
                || (BCS_Undefine == g_Bluetooth->connectStatus())
                || (BCS_Connecting == g_Bluetooth->connectStatus())) {
            if (isVisible()) {
                if (WidgetStatus::RequestShow == status) {
                    g_Widget->setWidgetType(Widget::T_ViewPaper0, Widget::T_Bluetooth, WidgetStatus::RequestShow);
                }
            }
        }
        break;
    }
    case Widget::T_Bluetooth: {
        if (WidgetStatus::RequestShow == status) {
            if (Widget::T_Undefine == d->m_Type) {
                d->initializeBluetoothToolWidget();
                d->initializeBluetoothSettingWidget();
            } else {
                switch (g_Link->getLinkType()) {
                case CARPLAY:
                case CARLIFE:
                case AUTO: {
                    switch (g_Link->getLinkStatus()) {
                    case LINK_STARTING:
                    case LINK_SUCCESS:
                    case LINK_EXITING:
                    case LINK_EXITED: {
                        d->m_Type = Widget::T_BluetoothSetting;
                        break;
                    }
                    default: {
                        break;
                    }
                    }
                    break;
                }
                default: {
                    break;
                }
                }
                g_Widget->setWidgetType(d->m_Type, Widget::T_Bluetooth, WidgetStatus::RequestShow);
            }
        }
        break;
    }
    case Widget::T_Link: {
        if (WidgetStatus::Show == status) {
            connectSignalAndSlotByNamesake(g_Link, this, ARKRECEIVER(onLinkStatusChange(const int, const int)));
            setVisible(false);
        }
        break;
    }
    case Widget::T_BluetoothDial:
    case Widget::T_BluetoothContact:
    case Widget::T_BluetoothRecord:
    case Widget::T_BluetoothMusic:
    case Widget::T_BluetoothSetting: {
        if (WidgetStatus::RequestShow == status) {
            switch (destinationType) {
            case Widget::T_BluetoothDial: {
                d->initializeBluetoothToolWidget();
                d->initializeBluetoothDialWidget();
                break;
            }
            case Widget::T_BluetoothContact: {
                bool flag = (NULL == d->m_BluetoothContactWidget);
                d->initializeBluetoothContactWidget();
                d->initializeBluetoothRecordWidget();
                if (flag) {
                    g_Widget->setWidgetType(Widget::T_BluetoothContact, Widget::T_Bluetooth, WidgetStatus::RequestShow);
                }
                break;
            }
            case Widget::T_BluetoothRecord: {
                bool flag = (NULL == d->m_BluetoothRecordWidget);
                d->initializeBluetoothContactWidget();
                d->initializeBluetoothRecordWidget();
                if (flag) {
                    g_Widget->setWidgetType(Widget::T_BluetoothRecord, Widget::T_Bluetooth, WidgetStatus::RequestShow);
                }
                break;
            }
            case Widget::T_BluetoothMusic: {
                d->initializeBluetoothToolWidget();
                d->initializeBluetoothMusicWidget();
                break;
            }
            case Widget::T_BluetoothSetting: {
                d->initializeBluetoothToolWidget();
                d->initializeBluetoothSettingWidget();
                break;
            }
            default: {
                break;
            }
            }
        } else if (WidgetStatus::Show == status) {
            d->m_Type = destinationType;
            if (!isVisible()) {
                setVisible(true);
            }
        }
        break;
    }
    default: {
        if (WidgetStatus::Show == status) {
            setVisible(false);
        }
        break;
    }
    }
}

void BluetoothWidget::onConnectStatusChange(const BluetoothConnectStatus status)
{
    qDebug() << __PRETTY_FUNCTION__ << __LINE__ << status;
    switch (status) {
    case BCS_Incoming:
    case BCS_Outgoing:
    case BCS_Talking: {
        g_Widget->acquireWidgetType(Widget::T_BluetoothDial);
        break;
    }
    default: {
        g_Widget->releaseWidgetType(Widget::T_BluetoothDial);
        Q_D(BluetoothWidget);
        if (NULL != d->m_BluetoothMusicWidget) {
            if (NULL == d->m_BluetoothSettingWidget) {
                d->m_BluetoothSettingWidget = new BluetoothSettingWidget(this);
                d->m_BluetoothSettingWidget->setVisible(true);
                d->m_Type = Widget::T_BluetoothSetting;
            }
        }
        if ((status == BCS_Idle)
                || (status == BCS_Connecting)) {
            d->m_Type = Widget::T_BluetoothSetting;
        }
        switch (g_Link->getLinkStatus()) {
        case LINK_STARTING:
        case LINK_SUCCESS:
        case LINK_EXITING: {
            if (CARLIFE == g_Link->getLinkType()) {
                if (status == BCS_Idle) {
                    d->initializeMessageBox();
                    d->m_MessageBox->setText(SourceString::Use_Carlife_Device_Connect_Local_Bluetooth_Device);
                    d->m_MessageBox->setVisible(true);
                    d->initializeDevice();
                    d->m_Device->setVisible(true);
                    d->initializePin();
                    d->m_PinCode->setVisible(true);
                    d->initializeDisconnectBtn();
                    d->m_DisconnectBtn->setVisible(false);
                    g_Link->reqeustBTInfo(CARLIFE, 1);
                } else if (status == BCS_Connected) {
                    d->initializeMessageBox();
                    d->m_MessageBox->setText(SourceString::Switch_Carlife_Widget_To_Use_Bluetooth_Function);
                    d->m_MessageBox->setVisible(true);
                    d->initializeDevice();
                    d->m_Device->setVisible(true);
                    d->initializePin();
                    d->m_PinCode->setVisible(true);
                    d->initializeDisconnectBtn();
                    d->initializeDisconnectBtn();
                    d->m_DisconnectBtn->setVisible(true);
                    g_Link->reqeustBTInfo(CARLIFE, 2);
                }
            } else if (AUTO == g_Link->getLinkType()) {
                if (status == BCS_Idle) {
                    d->initializeMessageBox();
                    d->m_MessageBox->setText(SourceString::Use_Auto_Device_Connect_Local_Bluetooth_Device);
                    d->m_MessageBox->setVisible(true);
                    d->initializeDevice();
                    d->m_Device->setVisible(true);
                    d->initializePin();
                    d->m_PinCode->setVisible(true);
                    d->initializeDisconnectBtn();
                    d->m_DisconnectBtn->setVisible(false);
                } else if (status == BCS_Connected) {
                    d->initializeMessageBox();
                    d->m_MessageBox->setText(SourceString::Switch_Auto_Widget_To_Use_Bluetooth_Function);
                    d->m_MessageBox->setVisible(true);
                    d->initializeDevice();
                    d->m_Device->setVisible(true);
                    d->initializePin();
                    d->m_PinCode->setVisible(true);
                    d->initializeDisconnectBtn();
                    d->initializeDisconnectBtn();
                    d->m_DisconnectBtn->setVisible(true);
                }
                if (AS_AutoPhone != g_Audio->getAudioSource()) {
                    d->autoPaireHandler();
                }
            }
            break;
        }
        default: {
            break;
        }
        }
        break;
    }
    }
}

void BluetoothWidget::onLinkStatusChange(const Link_Type type, const Link_STATUS status)
{
    Q_D(BluetoothWidget);
    if (CARPLAY == type) {
        switch (status) {
        case LINK_STARTING:
        case LINK_SUCCESS:
        case LINK_EXITING: {
            d->initializeMessageBox();
            d->m_MessageBox->setText(SourceString::Turn_Off_Bluetooth_When_Connect_Carplay);
            d->m_MessageBox->setVisible(true);
            qDebug() << __PRETTY_FUNCTION__ << __LINE__ << d->m_RestoreBluetooth;
            if (BPM_PairedMode == g_Bluetooth->getPowerStatus()) {
                g_Bluetooth->powerOff();
                d->m_RestoreBluetooth = true;
            }
            break;
        }
        case LINK_FAIL:
        case LINK_REMOVED: {
            if (NULL != d->m_MessageBox) {
                d->m_MessageBox->setVisible(false);
            }
            qDebug() << __PRETTY_FUNCTION__ << __LINE__ << d->m_RestoreBluetooth;
            if (d->m_RestoreBluetooth) {
                d->m_RestoreBluetooth = false;
                g_Bluetooth->powerOn();
            }
            break;
        }
        default : {
            break;
        }
        }
    } else if (CARLIFE == type) {
        switch (status) {
        case LINK_STARTING:
        case LINK_SUCCESS:
        case LINK_EXITING: {
            g_Bluetooth->enableTrusteeshipNativeBluetooth();
            if (BPM_CancelPairedMode == g_Bluetooth->getPowerStatus()) {
                g_Link->reqeustBTInfo(CARLIFE, 0);
                g_Bluetooth->powerOn();
                d->m_RestoreBluetooth = true;
            }
            d->initializeMessageBox();
            if (BCS_Connected == g_Bluetooth->connectStatus()) {
                d->m_MessageBox->setText(SourceString::Switch_Carlife_Widget_To_Use_Bluetooth_Function);
            } else {
                d->m_MessageBox->setText(SourceString::Use_Carlife_Device_Connect_Local_Bluetooth_Device);
            }
            d->initializeDisconnectBtn();
            d->m_DisconnectBtn->setVisible(BCS_Connected == g_Bluetooth->connectStatus());
            d->m_MessageBox->setVisible(true);
            d->initializeDevice();
            d->m_Device->setText(QObject::tr(SourceString::Device.toLocal8Bit().constData()) + g_Bluetooth->localDeviceName());
            d->m_Device->setVisible(true);
            d->initializePin();
            d->m_PinCode->setText(QObject::tr(SourceString::Pin_Code.toLocal8Bit().constData()) + g_Bluetooth->pinCode());
            d->m_PinCode->setVisible(true);
            break;
        }
        case LINK_BT_DISCONNECT: {
            g_Bluetooth->disconnectRemoteDevice();
            break;
        }
        case LINK_FAIL:
        case LINK_REMOVED: {
            g_Bluetooth->disableTrusteeshipNativeBluetooth();
            if (NULL != d->m_Device) {
                d->m_Device->setVisible(false);
            }
            if (NULL != d->m_PinCode) {
                d->m_PinCode->setVisible(false);
            }
            if (NULL != d->m_DisconnectBtn) {
                d->m_DisconnectBtn->setVisible(false);
            }
            if (NULL != d->m_MessageBox) {
                d->m_MessageBox->setVisible(false);
            }
            if (d->m_RestoreBluetooth) {
                d->m_RestoreBluetooth = false;
                g_Bluetooth->powerOff();
            }
            break;
        }
        default: {
            break;
        }
        }
    } else if (AUTO == type) {
        switch (status) {
        case LINK_STARTING:
        case LINK_SUCCESS:
        case LINK_EXITING: {
            g_Bluetooth->enableTrusteeshipNativeBluetooth();
            d->initializeMessageBox();
            if (BCS_Connected == g_Bluetooth->connectStatus()) {
                d->m_MessageBox->setText(SourceString::Switch_Auto_Widget_To_Use_Bluetooth_Function);
            } else {
                d->m_MessageBox->setText(SourceString::Use_Auto_Device_Connect_Local_Bluetooth_Device);
            }
            d->m_MessageBox->setVisible(true);
            d->initializeDevice();
            d->m_Device->setText(QObject::tr(SourceString::Device.toLocal8Bit().constData()) + g_Bluetooth->localDeviceName());
            d->m_Device->setVisible(true);
            d->initializePin();
            d->m_PinCode->setText(QObject::tr(SourceString::Pin_Code.toLocal8Bit().constData()) + g_Bluetooth->pinCode());
            d->m_PinCode->setVisible(true);
            if (BPM_CancelPairedMode == g_Bluetooth->getPowerStatus()) {
                g_Bluetooth->powerOn();
                d->m_RestoreBluetooth = true;
            }
            d->initializeDisconnectBtn();
            d->m_DisconnectBtn->setVisible(BCS_Connected == g_Bluetooth->connectStatus());
            break;
        }
        case LINK_FAIL:
        case LINK_REMOVED: {
            g_Bluetooth->disableTrusteeshipNativeBluetooth();
            if (NULL != d->m_Device) {
                d->m_Device->setVisible(false);
            }
            if (NULL != d->m_PinCode) {
                d->m_PinCode->setVisible(false);
            }
            if (NULL != d->m_DisconnectBtn) {
                d->m_DisconnectBtn->setVisible(false);
            }
            if (NULL != d->m_MessageBox) {
                d->m_MessageBox->setVisible(false);
            }
            if (NULL != d->m_Timer) {
                d->m_Timer->stop();
            }
            if (d->m_RestoreBluetooth) {
                d->m_RestoreBluetooth = false;
                g_Bluetooth->powerOff();
            }
            break;
        }
        case LINK_AUTO_BT_REQUEST: {
            bool paired(false);
            if (BCS_Connected == g_Bluetooth->connectStatus()) {
                QFile file(QString(PHONEBTADDR));
                if (file.exists()) {
                    if (file.open(QIODevice::ReadOnly)) {
                        QString phoneaddr(file.readAll().data());
                        paired = phoneaddr == g_Bluetooth->remoteDeviceAddress();
                        qDebug() << "phoneaddr" << phoneaddr;
                        qDebug() << "remoteDeviceAddress()" << g_Bluetooth->remoteDeviceAddress();
                    }
                } else {
                    qDebug() << "PHONEBTADDR not exists!";
                }
            } else if (BCS_Connecting == g_Bluetooth->connectStatus()) {
                qDebug() << "bluetooth connecting!";
                return ;
            } else {
                qDebug() << "bluetooth not connted!";
            }
            if (paired) {
                g_Link->requestLinkStatus(AUTO, LINK_AUTO_BT_PAIRED);
            } else {
                d->initializeTimer();
                d->m_Timer->start();
            }
            break;
        }
        default: {
            break;
        }
        }
    }
}

void BluetoothWidget::onPowerChange(const BluetoothPairedMode mode)
{
    if (BPM_PairedMode == mode) {
        g_Link->reqeustBTInfo(CARLIFE, 1);
    }
}

void BluetoothWidget::onToolButtonRelease()
{
    g_Bluetooth->disconnectRemoteDevice();
}

void BluetoothWidget::onTimeout()
{
    Q_D(BluetoothWidget);
    d->autoPaireHandler();
}

BluetoothWidgetPrivate::BluetoothWidgetPrivate(BluetoothWidget *parent)
    : q_ptr(parent)
{
    m_MessageBox = NULL;
    m_BluetoothDialWidget = NULL;
    m_BluetoothContactWidget = NULL;
    m_BluetoothRecordWidget = NULL;
    m_BluetoothMusicWidget = NULL;
    m_BluetoothSettingWidget = NULL;
    m_BluetoothToolWidget = NULL;
    m_RestoreBluetooth = false;
    m_Device = NULL;
    m_PinCode = NULL;
    m_DisconnectBtn = NULL;
    m_Type = Widget::T_Undefine;
    m_Timer = NULL;
    initializeParent();
    connectAllSlots();
}

BluetoothWidgetPrivate::~BluetoothWidgetPrivate()
{
}

void BluetoothWidgetPrivate::initializeParent()
{
    Q_Q(BluetoothWidget);
    q->setVisible(false);
}

void BluetoothWidgetPrivate::connectAllSlots()
{
    Q_Q(BluetoothWidget);
    connectSignalAndSlotByNamesake(g_Widget, q, ARKRECEIVER(onWidgetTypeChange(const Widget::Type, const Widget::Type, const QString &)));
    connectSignalAndSlotByNamesake(g_Bluetooth, q, ARKRECEIVER(onConnectStatusChange(const BluetoothConnectStatus)));
}

void BluetoothWidgetPrivate::initializeMessageBox()
{
    if (NULL == m_MessageBox) {
        Q_Q(BluetoothWidget);
        m_MessageBox = new MessageBox(q);
        m_MessageBox->setVisible(false);
        m_MessageBox->setAutoHide(false);
        m_MessageBox->setFontPointSize(15 * g_Widget->widthScalabilityFactor());
        m_MessageBox->setBackgroundBmpPath(QString(":/Images/MessageBoxBackground.png"));
        g_Widget->geometryFit(0, 0, 800, 480, m_MessageBox);
    }
}

void BluetoothWidgetPrivate::initializeDevice()
{
    if (NULL == m_Device) {
        Q_Q(BluetoothWidget);
        m_Device = new TextWidget(q);
        m_Device->setLanguageType(TextWidget::T_NoTranslate);
        g_Widget->geometryFit(170, 110, 230, 130, m_Device);
    }
}

void BluetoothWidgetPrivate::initializePin()
{
    if (NULL == m_PinCode) {
        Q_Q(BluetoothWidget);
        m_PinCode = new TextWidget(q);
        m_PinCode->setLanguageType(TextWidget::T_NoTranslate);
        g_Widget->geometryFit(400, 110, 230, 130, m_PinCode);
    }
}

void BluetoothWidgetPrivate::initializeDisconnectBtn()
{
    if (NULL == m_DisconnectBtn) {
        Q_Q(BluetoothWidget);
        m_DisconnectBtn = new BmpButton(q);
        m_DisconnectBtn->setNormalBmpPath(QString(":/Images/BluetoothSettingDisconnectNormal.png"));
        m_DisconnectBtn->setPressBmpPath(QString(":/Images/BluetoothSettingDisconnectPress.png"));
        g_Widget->geometryFit(356, 110 + 173, 88, 86, m_DisconnectBtn);
        QObject::connect(m_DisconnectBtn, ARKSENDER(released()),
                         q,               ARKRECEIVER(onToolButtonRelease()));
        m_DisconnectBtn->setVisible(true);
    }
}

void BluetoothWidgetPrivate::initializeBluetoothDialWidget()
{
    if (NULL == m_BluetoothDialWidget) {
        Q_Q(BluetoothWidget);
        m_BluetoothDialWidget = new BluetoothDialWidget(q);
        g_Widget->setWidgetType(Widget::T_BluetoothDial, Widget::T_Bluetooth, WidgetStatus::RequestShow);
    }
}

void BluetoothWidgetPrivate::initializeBluetoothContactWidget()
{
    if (NULL == m_BluetoothContactWidget) {
        Q_Q(BluetoothWidget);
        m_BluetoothContactWidget = new BluetoothContactWidget(q);
    }
}

void BluetoothWidgetPrivate::initializeBluetoothRecordWidget()
{
    if (NULL == m_BluetoothRecordWidget) {
        Q_Q(BluetoothWidget);
        m_BluetoothRecordWidget = new BluetoothRecordWidget(q);
    }
}

void BluetoothWidgetPrivate::initializeBluetoothMusicWidget()
{
    if (NULL == m_BluetoothMusicWidget) {
        Q_Q(BluetoothWidget);
        m_BluetoothMusicWidget = new BluetoothMusicWidget(q);
        g_Widget->setWidgetType(Widget::T_BluetoothMusic, Widget::T_Bluetooth, WidgetStatus::RequestShow);
    }
}

void BluetoothWidgetPrivate::initializeBluetoothSettingWidget()
{
    if (NULL == m_BluetoothSettingWidget) {
        Q_Q(BluetoothWidget);
        m_BluetoothSettingWidget = new BluetoothSettingWidget(q);
        g_Widget->setWidgetType(Widget::T_BluetoothSetting, Widget::T_Bluetooth, WidgetStatus::RequestShow);
    }
}

void BluetoothWidgetPrivate::initializeBluetoothToolWidget()
{
    if (NULL == m_BluetoothToolWidget) {
        Q_Q(BluetoothWidget);
        m_BluetoothToolWidget = new BluetoothToolWidget(q);
    }
}

void BluetoothWidgetPrivate::autoPaireHandler()
{
    bool paired(false);
    qDebug() << "asdasd" << g_Bluetooth->connectStatus();
    qDebug() << __PRETTY_FUNCTION__ << __LINE__;
    if (BCS_Connected == g_Bluetooth->connectStatus()) {
        qDebug() << __PRETTY_FUNCTION__ << __LINE__;
        QFile file(QString(PHONEBTADDR));
        if (file.exists()) {
            qDebug() << __PRETTY_FUNCTION__ << __LINE__;
            if (file.open(QIODevice::ReadOnly)) {
                qDebug() << __PRETTY_FUNCTION__ << __LINE__;
                QString phoneaddr(file.readAll().data());
                paired = phoneaddr == g_Bluetooth->remoteDeviceAddress();
                qDebug() << "phoneaddr" << phoneaddr;
                qDebug() << "remoteDeviceAddress()" << g_Bluetooth->remoteDeviceAddress();
            }
        } else {
            qDebug() << "PHONEBTADDR not exists!";
        }
    } else {
        qDebug() << "bluetooth not connted!";
    }
    if (paired) {
        qDebug() << __PRETTY_FUNCTION__ << __LINE__;
        g_Link->requestLinkStatus(AUTO, LINK_AUTO_BT_PAIRED);
    } else {
        qDebug() << __PRETTY_FUNCTION__ << __LINE__;
        if (BCS_Connected == g_Bluetooth->connectStatus()) {
            qDebug() << __PRETTY_FUNCTION__ << __LINE__;
            g_Bluetooth->disconnectRemoteDevice();
        }
        g_Link->requestLinkStatus(AUTO, LINK_AUTO_BT_UNPAIRED);
    }
}

void BluetoothWidgetPrivate::initializeTimer()
{
    if (NULL == m_Timer) {
        Q_Q(BluetoothWidget);
        m_Timer = new QTimer(q);
        m_Timer->setInterval(4444);
        m_Timer->setSingleShot(true);
        QObject::connect(m_Timer, ARKSENDER(timeout()),
                         q,       ARKRECEIVER(onTimeout()));
    }
}

