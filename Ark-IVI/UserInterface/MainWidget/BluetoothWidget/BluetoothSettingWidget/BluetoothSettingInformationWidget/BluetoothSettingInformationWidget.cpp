#include "BluetoothSettingInformationWidget.h"
#include "AutoConnect.h"
#include "BmpWidget.h"
#include "TextWidget.h"
#include "BmpButton.h"
#include "BusinessLogic/Widget.h"
#include "BusinessLogic/Bluetooth.h"
#include "AutoConnect.h"
#include <QDebug>
#include <QMouseEvent>

namespace SourceString {
static const QString Device(QObject::tr("Device:"));
static const QString Pin_Code(QObject::tr("PinCode:"));
static const QString Power(QObject::tr("Power:"));
static const QString AutoConnect(QObject::tr("Connect:"));
static const QString AutoAnswer(QObject::tr("Answer:"));
static const QString Off(QObject::tr("Off"));
static const QString On(QObject::tr("On"));
}

class BluetoothSettingInformationWidgetPrivate
{
public:
    explicit BluetoothSettingInformationWidgetPrivate(BluetoothSettingInformationWidget* parent);
    ~BluetoothSettingInformationWidgetPrivate();
    void initializeParent();
    void initializeBasicWidget();
    void connectAllSlots();
private:
    Q_DECLARE_PUBLIC(BluetoothSettingInformationWidget)
    BluetoothSettingInformationWidget* const q_ptr;
    BmpWidget* m_Background;
    BmpWidget* m_BluetoothIcon;
    TextWidget* m_DeviceName;
    TextWidget* m_PinCode;
    TextWidget* m_Power;
    BmpButton* m_PowerBtn;
    TextWidget* m_AutoConnect;
    BmpButton* m_AutoConnectBtn;
    TextWidget* m_AutoAccept;
    BmpButton* m_AutoAnswerBtn;
};

BluetoothSettingInformationWidget::BluetoothSettingInformationWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new BluetoothSettingInformationWidgetPrivate(this))
{
}

BluetoothSettingInformationWidget::~BluetoothSettingInformationWidget()
{
    if (NULL != d_ptr) {
        delete d_ptr;
    }
}

void BluetoothSettingInformationWidget::setDeviceName(const QString &name)
{
    Q_D(BluetoothSettingInformationWidget);
    d->m_DeviceName->setText(QObject::tr(SourceString::Device.toLocal8Bit().constData()) + name);
}

void BluetoothSettingInformationWidget::setPinCode(const QString &pin)
{
    Q_D(BluetoothSettingInformationWidget);
    d->m_PinCode->setText(QObject::tr(SourceString::Pin_Code.toLocal8Bit().constData()) + pin);
}

void BluetoothSettingInformationWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(29, 123, 436, 260, this);
}

void BluetoothSettingInformationWidget::mouseReleaseEvent(QMouseEvent *event)
{
    Q_D(BluetoothSettingInformationWidget);
    // if (d->m_DeviceName->geometry().contains(event->pos())) {
    //     popAlphabetPad();
    // } else if (d->m_PinCode->geometry().contains(event->pos())) {
    //     popNumberPad();
    // }
}

void BluetoothSettingInformationWidget::onPowerChange(const BluetoothPairedMode mode)
{
    Q_D(BluetoothSettingInformationWidget);
    if (BPM_PairedMode == mode) {
        d->m_PowerBtn->setText(SourceString::On);
    } else if (BPM_CancelPairedMode == mode) {
        d->m_PowerBtn->setText(SourceString::Off);
    }
}

void BluetoothSettingInformationWidget::onAutoConnectChange(const BluetoothAutoConnect mode)
{
    Q_D(BluetoothSettingInformationWidget);
    if (BAC_AutoConnect == mode) {
        d->m_AutoConnectBtn->setText(SourceString::On);
    } else if (BAC_CancelAutoConnect == mode) {
        d->m_AutoConnectBtn->setText(SourceString::Off);
    }
}

void BluetoothSettingInformationWidget::onAutoAnswerChange(const BluetoothAutoAnswer mode)
{
    Q_D(BluetoothSettingInformationWidget);
    if (BAA_AutoAnswer == mode) {
        d->m_AutoAnswerBtn->setText(SourceString::On);
    } else if (BAA_CancelAutoAnswer == mode) {
        d->m_AutoAnswerBtn->setText(SourceString::Off);
    }
}

void BluetoothSettingInformationWidget::onBmpButtonRelease()
{
    qDebug() << __PRETTY_FUNCTION__ << __LINE__;
    BmpButton* ptr = static_cast<BmpButton*>(sender());
    Q_D(BluetoothSettingInformationWidget);
    if (ptr == d->m_PowerBtn) {
        qDebug() << __PRETTY_FUNCTION__ << __LINE__;
        BluetoothPairedMode status = g_Bluetooth->getPowerStatus();
        if (BPM_PairedMode == status) {
            g_Bluetooth->powerOff();
        } else if (BPM_CancelPairedMode == status) {
            g_Bluetooth->powerOn();
        }
    } else if (ptr == d->m_AutoConnectBtn) {
        qDebug() << __PRETTY_FUNCTION__ << __LINE__;
        //        g_Bluetooth->setLocalDeviceName(QString("ABCD"));
        BluetoothAutoConnect status = g_Bluetooth->getAutoConnectStatus();
        if (BAC_AutoConnect == status) {
            g_Bluetooth->autoConnectOff();
        } else if (BAC_CancelAutoConnect == status) {
            g_Bluetooth->autoConnectOn();
        }
    } else if (ptr == d->m_AutoAnswerBtn) {
        BluetoothAutoAnswer status = g_Bluetooth->getAutoAnswer();
        if (BAA_AutoAnswer == status) {
            g_Bluetooth->autoAnswerOff();
        } else if (BAA_CancelAutoAnswer == status) {
            g_Bluetooth->autoAnswerOn();
        }
    }
}

BluetoothSettingInformationWidgetPrivate::BluetoothSettingInformationWidgetPrivate(BluetoothSettingInformationWidget *parent)
    : q_ptr(parent)
{
    m_Background = NULL;
    m_BluetoothIcon = NULL;
    m_DeviceName = NULL;
    m_PinCode = NULL;
    m_Power = NULL;
    m_PowerBtn = NULL;
    m_AutoConnect = NULL;
    m_AutoConnectBtn = NULL;
    m_AutoAccept = NULL;
    m_AutoAnswerBtn = NULL;
    initializeParent();
    initializeBasicWidget();
    connectAllSlots();
}

BluetoothSettingInformationWidgetPrivate::~BluetoothSettingInformationWidgetPrivate()
{
}

void BluetoothSettingInformationWidgetPrivate::initializeParent()
{
    Q_Q(BluetoothSettingInformationWidget);
    q->setVisible(true);
}

void BluetoothSettingInformationWidgetPrivate::initializeBasicWidget()
{
    Q_Q(BluetoothSettingInformationWidget);
    m_Background = new BmpWidget(q);
    m_Background->setBackgroundBmpPath(QString(":/Images/BluetoothSettingInformationBackground.png"));
    g_Widget->geometryFit(0, 0, 436, 260, m_Background);
    m_Background->setVisible(true);

    m_BluetoothIcon = new BmpWidget(q);
    m_BluetoothIcon->setBackgroundBmpPath(QString(":/Images/BluetoothSettingIcon.png"));
    g_Widget->geometryFit(43, 28, 118, 230, m_BluetoothIcon);
    m_BluetoothIcon->setVisible(true);

    m_DeviceName = new TextWidget(q);
    m_DeviceName->setAlignmentFlag(Qt::AlignLeft | Qt::AlignVCenter);
    m_DeviceName->setLanguageType(TextWidget::T_NoTranslate);
    g_Widget->geometryFit(218, 5, 218, 46, m_DeviceName);
    m_DeviceName->setVisible(true);

    m_PinCode = new TextWidget(q);
    m_PinCode->setAlignmentFlag(Qt::AlignLeft | Qt::AlignVCenter);
    m_PinCode->setLanguageType(TextWidget::T_NoTranslate);
    g_Widget->geometryFit(218, 56, 218, 46, m_PinCode);
    m_PinCode->setVisible(true);

    m_Power = new TextWidget(q);
    m_Power->setAlignmentFlag(Qt::AlignLeft | Qt::AlignVCenter);
    m_Power->setText(SourceString::Power);
    g_Widget->geometryFit(218, 107, 109, 46, m_Power);
    m_Power->setVisible(true);

    m_PowerBtn = new BmpButton(q);
    m_PowerBtn->setNormalBmpPath(QString(":/Images/SettingWidgetBtnNormal.png"));
    m_PowerBtn->setPressBmpPath(QString(":/Images/SettingWidgetBtnPress.png"));
    BluetoothPairedMode powerMode = g_Bluetooth->getPowerStatus();
    if (BPM_PairedMode == powerMode) {
        m_PowerBtn->setText(SourceString::On);
    } else if (BPM_CancelPairedMode == powerMode) {
        m_PowerBtn->setText(SourceString::Off);
    }
    g_Widget->geometryFit(327, 107, 109, 46, m_PowerBtn);
    m_PowerBtn->setVisible(true);
    QObject::connect(m_PowerBtn, ARKSENDER(released()),
                     q,           ARKRECEIVER(onBmpButtonRelease()));

    m_AutoConnect = new TextWidget(q);
    m_AutoConnect->setAlignmentFlag(Qt::AlignLeft | Qt::AlignVCenter);
    m_AutoConnect->setText(SourceString::AutoConnect);
    g_Widget->geometryFit(218, 158, 109, 46, m_AutoConnect);
    m_AutoConnect->setVisible(true);

    m_AutoConnectBtn = new BmpButton(q);
    m_AutoConnectBtn->setNormalBmpPath(QString(":/Images/SettingWidgetBtnNormal.png"));
    m_AutoConnectBtn->setPressBmpPath(QString(":/Images/SettingWidgetBtnPress.png"));
    BluetoothAutoConnect autoConnect = g_Bluetooth->getAutoConnectStatus();
    if (BAC_AutoConnect == autoConnect) {
        m_AutoConnectBtn->setText(SourceString::On);
    } else if (BAC_CancelAutoConnect == autoConnect) {
        m_AutoConnectBtn->setText(SourceString::Off);
    }
    g_Widget->geometryFit(327, 158, 109, 46, m_AutoConnectBtn);
    m_AutoConnectBtn->setVisible(true);
    QObject::connect(m_AutoConnectBtn, ARKSENDER(released()),
                     q,                ARKRECEIVER(onBmpButtonRelease()));

    m_AutoAccept = new TextWidget(q);
    m_AutoAccept->setAlignmentFlag(Qt::AlignLeft | Qt::AlignVCenter);
    m_AutoAccept->setText(SourceString::AutoAnswer);
    g_Widget->geometryFit(218, 209, 109, 46, m_AutoAccept);
    m_AutoAccept->setVisible(true);
    m_AutoAnswerBtn = new BmpButton(q);
    m_AutoAnswerBtn->setNormalBmpPath(QString(":/Images/SettingWidgetBtnNormal.png"));
    m_AutoAnswerBtn->setPressBmpPath(QString(":/Images/SettingWidgetBtnPress.png"));
    BluetoothAutoAnswer autoAnswer = g_Bluetooth->getAutoAnswer();
    if (BAA_AutoAnswer == autoAnswer) {
        m_AutoAnswerBtn->setText(SourceString::On);
    } else if (BAA_CancelAutoAnswer == autoAnswer) {
        m_AutoAnswerBtn->setText(SourceString::Off);
    }
    g_Widget->geometryFit(327, 209, 109, 46, m_AutoAnswerBtn);
    m_AutoAnswerBtn->setVisible(true);
    QObject::connect(m_AutoAnswerBtn, ARKSENDER(released()),
                     q,               ARKRECEIVER(onBmpButtonRelease()));
}

void BluetoothSettingInformationWidgetPrivate::connectAllSlots()
{
    Q_Q(BluetoothSettingInformationWidget);
    connectSignalAndSlotByNamesake(g_Bluetooth, q, ARKRECEIVER(onPowerChange(const BluetoothPairedMode)));
    connectSignalAndSlotByNamesake(g_Bluetooth, q, ARKRECEIVER(onAutoConnectChange(const BluetoothAutoConnect)));
    connectSignalAndSlotByNamesake(g_Bluetooth, q, ARKRECEIVER(onAutoAnswerChange(const BluetoothAutoAnswer)));
}
