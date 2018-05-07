#include "BluetoothSettingWidget.h"
#include "AutoConnect.h"
#include "BluetoothSettingInformationWidget/BluetoothSettingInformationWidget.h"
#include "BluetoothSettingPairedListWidget/BluetoothSettingPairedListWidget.h"
#include "BluetoothSettingStatusWidget/BluetoothSettingStatusWidget.h"
#include "BluetoothSettingAlphabetPadWidget/BluetoothSettingAlphabetPadWidget.h"
#include "BluetoothSettingNumberPadWidget/BluetoothSettingNumberPadWidget.h"
#include <QDebug>

namespace SourceString {
static const QString Device(QObject::tr("Device:"));
static const QString Pin_Code(QObject::tr("PinCode:"));
}

class BluetoothSettingWidgetPrivate
{
public:
    explicit BluetoothSettingWidgetPrivate(BluetoothSettingWidget* parent);
    ~BluetoothSettingWidgetPrivate();
    void initializeParent();
    void initializeBasicWidget();
    void initializeBluetoothSettingAlphabetPadWidget();
    void initializeBluetoothSettingNumberPadWidget();
    void connectAllSlots();
private:
    Q_DECLARE_PUBLIC(BluetoothSettingWidget)
    BluetoothSettingWidget* const q_ptr;
    BluetoothSettingInformationWidget* m_BluetoothSettingInformationWidget;
    BluetoothSettingPairedListWidget* m_BluetoothSettingPairedListWidget;
    BluetoothSettingStatusWidget* m_BluetoothSettingStatusWidget;
    BluetoothSettingAlphabetPadWidget* m_BluetoothSettingAlphabetPadWidget;
    QString m_EditDeviceName;
    BluetoothSettingNumberPadWidget* m_BluetoothSettingNumberPadWidget;
    QString m_EditPincode;
};

BluetoothSettingWidget::BluetoothSettingWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new BluetoothSettingWidgetPrivate(this))
{
}

BluetoothSettingWidget::~BluetoothSettingWidget()
{
    if (NULL != d_ptr) {
        delete d_ptr;
    }
}

void BluetoothSettingWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(0, 0, 800, 480, this);
}

void BluetoothSettingWidget::onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status)
{
    Q_D(BluetoothSettingWidget);
    switch (destinationType) {
    case Widget::T_BluetoothSetting: {
        if (WidgetStatus::RequestShow == status) {
            g_Widget->setWidgetType(Widget::T_BluetoothSetting, requestType, WidgetStatus::Show);
        } else if (WidgetStatus::Show == status) {
            lower();
            d->m_BluetoothSettingStatusWidget->setConnectStatus(g_Bluetooth->connectStatus(), g_Bluetooth->remoteDeviceName());
            d->m_BluetoothSettingInformationWidget->setDeviceName(g_Bluetooth->localDeviceName());
            d->m_BluetoothSettingInformationWidget->setPinCode(g_Bluetooth->pinCode());
            setVisible(true);
        }
        break;
    }
    default: {
        if (WidgetStatus::Show == status) {
            if (isVisible()) {
                if (NULL != d->m_BluetoothSettingAlphabetPadWidget) {
                    d->m_BluetoothSettingAlphabetPadWidget->setVisible(false);
                    d->m_EditDeviceName.clear();
                    d->m_BluetoothSettingAlphabetPadWidget->setDeviceName(d->m_EditDeviceName);
                }
                if (NULL != d->m_BluetoothSettingNumberPadWidget) {
                    d->m_BluetoothSettingNumberPadWidget->setVisible(false);
                    d->m_EditPincode.clear();
                    d->m_BluetoothSettingNumberPadWidget->setPincode(d->m_EditPincode);
                }
                setVisible(false);
            }
        }
        break;
    }
    }
}

void BluetoothSettingWidget::onConnectStatusChange(const BluetoothConnectStatus status)
{
    qDebug() << __PRETTY_FUNCTION__ << status;
    Q_D(BluetoothSettingWidget);
    d->m_BluetoothSettingStatusWidget->setConnectStatus(status, g_Bluetooth->remoteDeviceName());
    d->m_BluetoothSettingPairedListWidget->setConnectStatus(status);
    if ((BCS_Idle == status)
            || (BCS_Connecting == status)) {
        setVisible(true);
    }
}

void BluetoothSettingWidget::onPinCodeChange(const QString &name)
{
    Q_D(BluetoothSettingWidget);
    d->m_BluetoothSettingInformationWidget->setPinCode(name);
}

void BluetoothSettingWidget::onLocalDeviceNameChange(const QString &name)
{
    Q_D(BluetoothSettingWidget);
    d->m_BluetoothSettingInformationWidget->setDeviceName(name);
}

void BluetoothSettingWidget::onRemoteDeviceNameChange(const QString &name)
{
    Q_D(BluetoothSettingWidget);
    d->m_BluetoothSettingStatusWidget->setConnectStatus(g_Bluetooth->connectStatus(), name);
}

void BluetoothSettingWidget::onAlphabetPadKeyType(const QString &type)
{
    Q_D(BluetoothSettingWidget);
    if (QString("Popdown") == type) {
        d->m_BluetoothSettingAlphabetPadWidget->setVisible(false);
        d->m_EditDeviceName.clear();
        d->m_BluetoothSettingAlphabetPadWidget->setDeviceName(d->m_EditDeviceName);
    } else if (QString("Confirm") == type) {
        if (d->m_EditDeviceName.count() > 0) {
            g_Bluetooth->setLocalDeviceName(d->m_EditDeviceName);
        }
        d->m_BluetoothSettingAlphabetPadWidget->setVisible(false);
        d->m_EditDeviceName.clear();
        d->m_BluetoothSettingAlphabetPadWidget->setDeviceName(d->m_EditDeviceName);
    } else if (QString("Backspace") == type) {
        if (d->m_EditDeviceName.length() > 0) {
            d->m_EditDeviceName = d->m_EditDeviceName.left(d->m_EditDeviceName.length() - 1);
            d->m_BluetoothSettingAlphabetPadWidget->setDeviceName(d->m_EditDeviceName);
        }
    } else {
        if (d->m_EditDeviceName.count() < 8) {
            d->m_EditDeviceName += type;
            d->m_BluetoothSettingAlphabetPadWidget->setDeviceName(d->m_EditDeviceName);
        }
    }
}

void BluetoothSettingWidget::onNumberPadKeyType(const QString &type)
{
    Q_D(BluetoothSettingWidget);
    if (QString("Popdown") == type) {
        d->m_BluetoothSettingNumberPadWidget->setVisible(false);
        d->m_EditPincode.clear();
        d->m_BluetoothSettingNumberPadWidget->setPincode(d->m_EditPincode);
    } else if (QString("Confirm") == type) {
        if (4 == d->m_EditPincode.count()) {
            g_Bluetooth->setPincode(d->m_EditPincode);
            d->m_EditPincode.clear();
        }
        if (0 == d->m_EditPincode.count()) {
            d->m_BluetoothSettingNumberPadWidget->setVisible(false);
            d->m_BluetoothSettingNumberPadWidget->setPincode(d->m_EditPincode);
        }
    } else if (QString("Backspace") == type) {
        if (d->m_EditPincode.length() > 0) {
            d->m_EditPincode = d->m_EditPincode.left(d->m_EditPincode.length() - 1);
            d->m_BluetoothSettingNumberPadWidget->setPincode(d->m_EditPincode);
        }
    } else {
        if (d->m_EditPincode.count() < 4) {
            d->m_EditPincode += type;
            d->m_BluetoothSettingNumberPadWidget->setPincode(d->m_EditPincode);
        }
    }
}

void BluetoothSettingWidget::onPopAlphabetPad()
{
    Q_D(BluetoothSettingWidget);
    d->initializeBluetoothSettingAlphabetPadWidget();
    d->m_EditDeviceName.clear();
    d->m_BluetoothSettingAlphabetPadWidget->setDeviceName(d->m_EditDeviceName);
    d->m_BluetoothSettingAlphabetPadWidget->setVisible(true);
}

void BluetoothSettingWidget::onPopNumberPad()
{
    Q_D(BluetoothSettingWidget);
    d->initializeBluetoothSettingNumberPadWidget();
    d->m_EditPincode.clear();
    d->m_BluetoothSettingNumberPadWidget->setPincode(d->m_EditPincode);
    d->m_BluetoothSettingNumberPadWidget->setVisible(true);
}

BluetoothSettingWidgetPrivate::BluetoothSettingWidgetPrivate(BluetoothSettingWidget *parent)
    : q_ptr(parent)
{
    m_BluetoothSettingInformationWidget = NULL;
    m_BluetoothSettingPairedListWidget = NULL;
    m_BluetoothSettingStatusWidget = NULL;
    m_BluetoothSettingAlphabetPadWidget = NULL;
    m_BluetoothSettingNumberPadWidget = NULL;
    initializeBasicWidget();
    initializeParent();
    connectAllSlots();
}

BluetoothSettingWidgetPrivate::~BluetoothSettingWidgetPrivate()
{
}

void BluetoothSettingWidgetPrivate::initializeParent()
{
    Q_Q(BluetoothSettingWidget);
    q->setVisible(false);
}

void BluetoothSettingWidgetPrivate::initializeBasicWidget()
{
    Q_Q(BluetoothSettingWidget);
    m_BluetoothSettingInformationWidget = new BluetoothSettingInformationWidget(q);
    QObject::connect(m_BluetoothSettingInformationWidget, ARKSENDER(popAlphabetPad()),
                     q,                                   ARKRECEIVER(onPopAlphabetPad()));
    QObject::connect(m_BluetoothSettingInformationWidget, ARKSENDER(popNumberPad()),
                     q,                                   ARKRECEIVER(onPopNumberPad()));
    m_BluetoothSettingPairedListWidget = new BluetoothSettingPairedListWidget(q);
    m_BluetoothSettingStatusWidget = new BluetoothSettingStatusWidget(q);
    m_BluetoothSettingStatusWidget->setConnectStatus(g_Bluetooth->connectStatus(), g_Bluetooth->remoteDeviceName());
    m_BluetoothSettingInformationWidget->setDeviceName(g_Bluetooth->localDeviceName());
    m_BluetoothSettingInformationWidget->setPinCode(g_Bluetooth->pinCode());
    q->lower();
}

void BluetoothSettingWidgetPrivate::initializeBluetoothSettingAlphabetPadWidget()
{
    if (NULL == m_BluetoothSettingAlphabetPadWidget) {
        Q_Q(BluetoothSettingWidget);
        m_BluetoothSettingAlphabetPadWidget = new BluetoothSettingAlphabetPadWidget(q);
        QObject::connect(m_BluetoothSettingAlphabetPadWidget, ARKSENDER(alphabetPadKeyType(const QString&)),
                         q,                                   ARKRECEIVER(onAlphabetPadKeyType(const QString&)));
    }
}

void BluetoothSettingWidgetPrivate::initializeBluetoothSettingNumberPadWidget()
{
    if (NULL == m_BluetoothSettingNumberPadWidget) {
        Q_Q(BluetoothSettingWidget);
        m_BluetoothSettingNumberPadWidget = new BluetoothSettingNumberPadWidget(q);
        QObject::connect(m_BluetoothSettingNumberPadWidget, ARKSENDER(numberPadKeyType(const QString&)),
                         q,                                 ARKRECEIVER(onNumberPadKeyType(const QString&)));
    }
}

void BluetoothSettingWidgetPrivate::connectAllSlots()
{
    Q_Q(BluetoothSettingWidget);
    connectSignalAndSlotByNamesake(g_Widget, q, ARKRECEIVER(onWidgetTypeChange(const Widget::Type, const Widget::Type, const QString &)));
    connectSignalAndSlotByNamesake(g_Bluetooth, q, ARKRECEIVER(onConnectStatusChange(const BluetoothConnectStatus)));
    connectSignalAndSlotByNamesake(g_Bluetooth, q, ARKRECEIVER(onPinCodeChange(const QString&)));
    connectSignalAndSlotByNamesake(g_Bluetooth, q, ARKRECEIVER(onLocalDeviceNameChange(const QString&)));
    connectSignalAndSlotByNamesake(g_Bluetooth, q, ARKRECEIVER(onRemoteDeviceNameChange(const QString&)));
}
