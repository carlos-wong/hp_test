#include "BluetoothDialWidget.h"
#include "AutoConnect.h"
#include "BmpButton.h"
#include "BmpWidget.h"
#include "TextWidget.h"
#include <QDebug>

class BluetoothDialWidgetPrivate
{
public:
    explicit BluetoothDialWidgetPrivate(BluetoothDialWidget* parent);
    ~BluetoothDialWidgetPrivate();
    void initializeParent();
    void initializeBasicWidget();
    void connectAllSlots();
private:
    Q_DECLARE_PUBLIC(BluetoothDialWidget)
    BluetoothDialWidget* const q_ptr;
    BluetoothConnectStatus m_BluetoothConnectStatus;
    BmpWidget* m_InfoBackground;
    TextWidget* m_PhoneText;
    TextWidget* m_NameText;
    QString m_DialNumber;
    BmpWidget* m_Avatar;
    TextWidget* m_DialText;
    BmpWidget* m_DialBackground;
    BmpButton* m_BackspaceBtn;
    BmpButton* m_AncceptBtn;
    BmpButton* m_HandUpBtn;
    BmpButton* m_VoiceSwitchBtn;
    BmpButton* m_DialOneBtn;
    BmpButton* m_DialTwoBtn;
    BmpButton* m_DialThreeBtn;
    BmpButton* m_DialFourBtn;
    BmpButton* m_DialFiveBtn;
    BmpButton* m_DialSixBtn;
    BmpButton* m_DialSevenBtn;
    BmpButton* m_DialEightBtn;
    BmpButton* m_DialNineBtn;
    BmpButton* m_DialAsteriskBtn;
    BmpButton* m_DialZeroBtn;
    BmpButton* m_DialPoundBtn;
};

BluetoothDialWidget::BluetoothDialWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new BluetoothDialWidgetPrivate(this))
{
}

BluetoothDialWidget::~BluetoothDialWidget()
{
    if (NULL != d_ptr) {
        delete d_ptr;
    }
}

void BluetoothDialWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(0, 0, 800, 480, this);
}

void BluetoothDialWidget::onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status)
{
    switch (destinationType) {
    case Widget::T_BluetoothContact:
    case Widget::T_BluetoothRecord:
    case Widget::T_BluetoothMusic:
    case Widget::T_BluetoothSetting: {
        if (WidgetStatus::Show == status) {
            setVisible(false);
        }
        break;
    }
    case Widget::T_BluetoothDial: {
        if (WidgetStatus::RequestShow == status) {
            g_Widget->setWidgetType(Widget::T_BluetoothDial, requestType, WidgetStatus::Show);
        } else if (WidgetStatus::Show == status) {
            lower();
            onConnectStatusChange(g_Bluetooth->connectStatus());
            setVisible(true);
        }
        break;
    }
    default: {
        break;
    }
    }
}

void BluetoothDialWidget::onConnectStatusChange(const BluetoothConnectStatus status)
{
    qDebug() << __PRETTY_FUNCTION__ << status;
    Q_D(BluetoothDialWidget);
    d->m_BluetoothConnectStatus = status;
    switch (status) {
    case BCS_Idle:
    case BCS_Connecting: {
        setVisible(false);
        break;
    }
    case BCS_Connected: {
        d->m_PhoneText->setText(QString());
        d->m_NameText->setText(QString());
        d->m_DialNumber.clear();
        d->m_BackspaceBtn->setDisabled(true);
        d->m_DialText->setText(d->m_DialNumber);
        d->m_AncceptBtn->setDisabled(false);
        d->m_AncceptBtn->setVisible(true);
        //        d->m_HandUpBtn->setDisabled(true);
        d->m_HandUpBtn->setVisible(true);
        d->m_VoiceSwitchBtn->setNormalBmpPath(QString(":/Images/BluetoothVioceBluetoothNormal.png"));
        d->m_VoiceSwitchBtn->setPressBmpPath(QString(":/Images/BluetoothVioceBluetoothPress.png"));
        d->m_VoiceSwitchBtn->setVisible(false);
        break;
    }
    case BCS_Outgoing: {
        d->m_AncceptBtn->setDisabled(true);
        d->m_AncceptBtn->setVisible(true);
        //        d->m_HandUpBtn->setDisabled(false);
        d->m_HandUpBtn->setVisible(true);
        //        d->m_VoiceSwitchBtn->setNormalBmpPath(QString(":/Images/BluetoothVioceBluetoothNormal.png"));
        //        d->m_VoiceSwitchBtn->setPressBmpPath(QString(":/Images/BluetoothVioceBluetoothPress.png"));
        //        d->m_VoiceSwitchBtn->setVisible(true);
        break;
    }
    case BCS_Incoming: {
        d->m_AncceptBtn->setDisabled(false);
        d->m_AncceptBtn->setVisible(true);
        //        d->m_HandUpBtn->setDisabled(false);
        d->m_HandUpBtn->setVisible(true);
        //        d->m_VoiceSwitchBtn->setNormalBmpPath(QString(":/Images/BluetoothVioceBluetoothNormal.png"));
        //        d->m_VoiceSwitchBtn->setPressBmpPath(QString(":/Images/BluetoothVioceBluetoothPress.png"));
        //        d->m_VoiceSwitchBtn->setVisible(true);
        break;
    }
    case BCS_Talking: {
        d->m_AncceptBtn->setDisabled(true);
        d->m_AncceptBtn->setVisible(true);
        //        d->m_HandUpBtn->setDisabled(false);
        d->m_HandUpBtn->setVisible(true);
        //        d->m_VoiceSwitchBtn->setNormalBmpPath(QString(":/Images/BluetoothVioceBluetoothNormal.png"));
        //        d->m_VoiceSwitchBtn->setPressBmpPath(QString(":/Images/BluetoothVioceBluetoothPress.png"));
        //        d->m_VoiceSwitchBtn->setVisible(true);
        break;
    }
    default: {
        break;
    }
    }
}

void BluetoothDialWidget::onVoiceChange(const BluetoothVoiceMode mode)
{
    qDebug() << __PRETTY_FUNCTION__ << mode;
    switch (mode) {
    case BVM_VoiceBluetooth: {
        Q_D(BluetoothDialWidget);
        d->m_VoiceSwitchBtn->setNormalBmpPath(QString(":/Images/BluetoothViocePhoneNormal.png"));
        d->m_VoiceSwitchBtn->setPressBmpPath(QString(":/Images/BluetoothViocePhonePress.png"));
        d->m_VoiceSwitchBtn->setVisible(true);
        break;
    }
    case BVM_VoicePhone: {
        Q_D(BluetoothDialWidget);
        d->m_VoiceSwitchBtn->setNormalBmpPath(QString(":/Images/BluetoothVioceBluetoothNormal.png"));
        d->m_VoiceSwitchBtn->setPressBmpPath(QString(":/Images/BluetoothVioceBluetoothPress.png"));
        d->m_VoiceSwitchBtn->setVisible(true);
        break;
    }
    default: {
        break;
    }
    }
}

void BluetoothDialWidget::onDialInfo(const BluetoothRecordType type, const QString &name, const QString &phone)
{
    qDebug() << __PRETTY_FUNCTION__ << name << phone;
    Q_D(BluetoothDialWidget);
    d->m_BackspaceBtn->setDisabled(true);
    d->m_PhoneText->setText(phone);
    d->m_NameText->setText(name);
    d->m_DialNumber.clear();
    d->m_DialText->setText(d->m_DialNumber);
}

void BluetoothDialWidget::onToolButtonRelease()
{
    BmpButton* ptr = static_cast<BmpButton*>(sender());
    Q_D(BluetoothDialWidget);
    if (ptr == d->m_VoiceSwitchBtn) {
        g_Bluetooth->voiceToggleSwitch();
    } else if (ptr == d->m_AncceptBtn) {
        if (d->m_DialNumber.length() > 0) {
            g_Bluetooth->dialPhone(d->m_DialNumber);
        } else {
            if (BCS_Connected == g_Bluetooth->connectStatus()) {
                g_Bluetooth->redialLastPhone();
            } else if (BCS_Incoming == g_Bluetooth->connectStatus()) {
                g_Bluetooth->pickupPhone();
            }
        }
    } else if (ptr == d->m_HandUpBtn) {
        g_Bluetooth->hanupPhone();
        if (BCS_Connected == g_Bluetooth->connectStatus()) {
            d->m_PhoneText->setText(QString());
            d->m_NameText->setText(QString());
            d->m_DialNumber.clear();
            d->m_DialText->setText(d->m_DialNumber);
        }
    } else {
        if (ptr == d->m_BackspaceBtn) {
            if (d->m_DialNumber.length() > 0) {
                d->m_DialNumber = d->m_DialNumber.left(d->m_DialNumber.length() - 1);
            } else {
                d->m_DialNumber.clear();
                d->m_BackspaceBtn->setDisabled(true);
            }
            d->m_DialText->setText(d->m_DialNumber);
        } else {
            d->m_DialNumber.append(ptr->objectName());
            d->m_BackspaceBtn->setDisabled(false);
            d->m_DialText->setText(d->m_DialNumber);
            if (BCS_Talking == d->m_BluetoothConnectStatus) {
                g_Bluetooth->dialNumber(d->m_DialNumber.at(d->m_DialNumber.length() - 1));
            }
        }
    }
}

void BluetoothDialWidget::onToolButtonLongPressed()
{
    Q_D(BluetoothDialWidget);
    d->m_DialNumber.clear();
    d->m_BackspaceBtn->setDisabled(true);
    d->m_DialText->setText(d->m_DialNumber);
}

BluetoothDialWidgetPrivate::BluetoothDialWidgetPrivate(BluetoothDialWidget *parent)
    : q_ptr(parent)
{
    m_BluetoothConnectStatus = BCS_Undefine;
    m_InfoBackground = NULL;
    m_PhoneText = NULL;
    m_NameText = NULL;
    m_Avatar = NULL;
    m_DialText = NULL;
    m_DialBackground = NULL;
    m_BackspaceBtn = NULL;
    m_AncceptBtn = NULL;
    m_HandUpBtn = NULL;
    m_VoiceSwitchBtn = NULL;
    m_DialOneBtn = NULL;
    m_DialTwoBtn = NULL;
    m_DialThreeBtn = NULL;
    m_DialFourBtn = NULL;
    m_DialFiveBtn = NULL;
    m_DialSixBtn = NULL;
    m_DialSevenBtn = NULL;
    m_DialEightBtn = NULL;
    m_DialNineBtn = NULL;
    m_DialAsteriskBtn = NULL;
    m_DialZeroBtn = NULL;
    m_DialPoundBtn = NULL;
    initializeParent();
    initializeBasicWidget();
    connectAllSlots();
}

BluetoothDialWidgetPrivate::~BluetoothDialWidgetPrivate()
{
}

void BluetoothDialWidgetPrivate::initializeParent()
{
    Q_Q(BluetoothDialWidget);
    q->setVisible(false);
}

void BluetoothDialWidgetPrivate::initializeBasicWidget()
{
    Q_Q(BluetoothDialWidget);
    Qt::ConnectionType type = static_cast<Qt::ConnectionType>(Qt::UniqueConnection | Qt::AutoConnection);
    m_InfoBackground = new BmpWidget(q);
    m_InfoBackground->setBackgroundBmpPath(QString(":/Images/BluetoothDialWidgetInfoBackground.png"));
    g_Widget->geometryFit(425, 74, 350, 312, m_InfoBackground);
    m_InfoBackground->setVisible(true);

    m_PhoneText = new TextWidget(q);
    m_PhoneText->setFontPointSize(16 * g_Widget->widthScalabilityFactor());
    m_PhoneText->setLanguageType(TextWidget::T_NoTranslate);
    g_Widget->geometryFit(443, 82, 210, 39, m_PhoneText);
    m_PhoneText->setAlignmentFlag(Qt::AlignLeft | Qt::AlignVCenter);
    m_PhoneText->setVisible(true);

    m_NameText = new TextWidget(q);
    m_NameText->setFontPointSize(16 * g_Widget->widthScalabilityFactor());
    m_NameText->setLanguageType(TextWidget::T_NoTranslate);
    g_Widget->geometryFit(443, 121, 210, 39, m_NameText);
    m_NameText->setAlignmentFlag(Qt::AlignLeft | Qt::AlignVCenter);
    m_NameText->setVisible(true);

    m_Avatar = new BmpWidget(q);
    m_Avatar->setBackgroundBmpPath(QString(":/Images/BluetoothDialWidgetInfoAvatar.png"));
    g_Widget->geometryFit(671, 82, 78, 78, m_Avatar);
    m_Avatar->setVisible(true);

    m_DialText = new TextWidget(q);
    m_DialText->setFontPointSize(16 * g_Widget->widthScalabilityFactor());
    m_DialText->setLanguageType(TextWidget::T_NoTranslate);
    m_DialText->setTextElideMode(Qt::ElideLeft);
    g_Widget->geometryFit(443, 175, 244, 52, m_DialText);
    m_DialText->setVisible(true);

    m_DialBackground = new BmpWidget(q);
    m_DialBackground->setBackgroundBmpPath(QString(":/Images/BluetoothDialBackgound.png"));
    g_Widget->geometryFit(443, 175, 244, 52, m_DialBackground);
    m_DialBackground->setVisible(true);

    m_BackspaceBtn = new BmpButton(q);
    m_BackspaceBtn->setNormalBmpPath(QString(":/Images/BluetoothBackspaceNormal.png"));
    m_BackspaceBtn->setPressBmpPath(QString(":/Images/BluetoothBackspacePress.png"));
    g_Widget->geometryFit(687, 175, 62, 52, m_BackspaceBtn);
    m_BackspaceBtn->setDisabled(true);
    m_BackspaceBtn->enableLongPress(true);
    m_BackspaceBtn->setVisible(true);
    QObject::connect(m_BackspaceBtn, ARKSENDER(longPressed()),
                     q,              ARKRECEIVER(onToolButtonLongPressed()),
                     type);
    QObject::connect(m_BackspaceBtn, ARKSENDER(released()),
                     q,              ARKRECEIVER(onToolButtonRelease()),
                     type);

    m_VoiceSwitchBtn = new BmpButton(q);
    g_Widget->geometryFit(531, 258, 138, 46, m_VoiceSwitchBtn);
    m_VoiceSwitchBtn->setVisible(true);
    QObject::connect(m_VoiceSwitchBtn, ARKSENDER(released()),
                     q,                ARKRECEIVER(onToolButtonRelease()),
                     type);

    m_AncceptBtn = new BmpButton(q);
    m_AncceptBtn->setNormalBmpPath(QString(":/Images/BluetoothAcceptNormal.png"));
    m_AncceptBtn->setPressBmpPath(QString(":/Images/BluetoothAcceptPress.png"));
    g_Widget->geometryFit(443, 312, 138, 46, m_AncceptBtn);
    m_AncceptBtn->setVisible(true);
    QObject::connect(m_AncceptBtn, ARKSENDER(released()),
                     q,            ARKRECEIVER(onToolButtonRelease()),
                     type);

    m_HandUpBtn = new BmpButton(q);
    m_HandUpBtn->setNormalBmpPath(QString(":/Images/BluetoothHandUpNormal.png"));
    m_HandUpBtn->setPressBmpPath(QString(":/Images/BluetoothHandUpPress.png"));
    g_Widget->geometryFit(618, 312, 138, 46, m_HandUpBtn);
    //    m_HandUpBtn->setDisabled(true);
    m_HandUpBtn->setVisible(true);
    QObject::connect(m_HandUpBtn, ARKSENDER(released()),
                     q,           ARKRECEIVER(onToolButtonRelease()),
                     type);

    m_DialOneBtn = new BmpButton(q);
    m_DialOneBtn->setObjectName(QString("1"));
    m_DialOneBtn->setNormalBmpPath(QString(":/Images/BluetoothDialOneNormal.png"));
    m_DialOneBtn->setPressBmpPath(QString(":/Images/BluetoothDialOnePress.png"));
    g_Widget->geometryFit(30 + (122 + 8) * 0, 52 + 22 + (74 + 0) * 0, 122, 74, m_DialOneBtn);
    m_DialOneBtn->setVisible(true);
    QObject::connect(m_DialOneBtn, ARKSENDER(released()),
                     q,            ARKRECEIVER(onToolButtonRelease()),
                     type);

    m_DialTwoBtn = new BmpButton(q);
    m_DialTwoBtn->setObjectName(QString("2"));
    m_DialTwoBtn->setNormalBmpPath(QString(":/Images/BluetoothDialTwoNormal.png"));
    m_DialTwoBtn->setPressBmpPath(QString(":/Images/BluetoothDialTwoPress.png"));
    g_Widget->geometryFit(30 + (122 + 8) * 1, 52 + 22 + (74 + 0) * 0, 122, 74, m_DialTwoBtn);
    m_DialTwoBtn->setVisible(true);
    QObject::connect(m_DialTwoBtn, ARKSENDER(released()),
                     q,            ARKRECEIVER(onToolButtonRelease()),
                     type);

    m_DialThreeBtn = new BmpButton(q);
    m_DialThreeBtn->setObjectName(QString("3"));
    m_DialThreeBtn->setNormalBmpPath(QString(":/Images/BluetoothDialThreeNormal.png"));
    m_DialThreeBtn->setPressBmpPath(QString(":/Images/BluetoothDialThreePress.png"));
    g_Widget->geometryFit(30 + (122 + 8) * 2, 52 + 22 + (74 + 0) * 0, 122, 74, m_DialThreeBtn);
    m_DialThreeBtn->setVisible(true);
    QObject::connect(m_DialThreeBtn, ARKSENDER(released()),
                     q,              ARKRECEIVER(onToolButtonRelease()),
                     type);

    m_DialFourBtn = new BmpButton(q);
    m_DialFourBtn->setObjectName(QString("4"));
    m_DialFourBtn->setNormalBmpPath(QString(":/Images/BluetoothDialFourNormal.png"));
    m_DialFourBtn->setPressBmpPath(QString(":/Images/BluetoothDialFourPress.png"));
    g_Widget->geometryFit(30 + (122 + 8) * 0, 52 + 22 + (74 + 4) * 1, 122, 74, m_DialFourBtn);
    m_DialFourBtn->setVisible(true);
    QObject::connect(m_DialFourBtn, ARKSENDER(released()),
                     q,             ARKRECEIVER(onToolButtonRelease()),
                     type);

    m_DialFiveBtn = new BmpButton(q);
    m_DialFiveBtn->setObjectName(QString("5"));
    m_DialFiveBtn->setNormalBmpPath(QString(":/Images/BluetoothDialFiveNormal.png"));
    m_DialFiveBtn->setPressBmpPath(QString(":/Images/BluetoothDialFivePress.png"));
    g_Widget->geometryFit(30 + (122 + 8) * 1, 52 + 22 + (74 + 4) * 1, 122, 74, m_DialFiveBtn);
    m_DialFiveBtn->setVisible(true);
    QObject::connect(m_DialFiveBtn, ARKSENDER(released()),
                     q,             ARKRECEIVER(onToolButtonRelease()),
                     type);

    m_DialSixBtn = new BmpButton(q);
    m_DialSixBtn->setObjectName(QString("6"));
    m_DialSixBtn->setNormalBmpPath(QString(":/Images/BluetoothDialSixNormal.png"));
    m_DialSixBtn->setPressBmpPath(QString(":/Images/BluetoothDialSixPress.png"));
    g_Widget->geometryFit(30 + (122 + 8) * 2, 52 + 22 + (74 + 4) * 1, 122, 74, m_DialSixBtn);
    m_DialSixBtn->setVisible(true);
    QObject::connect(m_DialSixBtn, ARKSENDER(released()),
                     q,            ARKRECEIVER(onToolButtonRelease()),
                     type);

    m_DialSevenBtn = new BmpButton(q);
    m_DialSevenBtn->setObjectName(QString("7"));
    m_DialSevenBtn->setNormalBmpPath(QString(":/Images/BluetoothDialSevenNormal.png"));
    m_DialSevenBtn->setPressBmpPath(QString(":/Images/BluetoothDialSevenPress.png"));
    g_Widget->geometryFit(30 + (122 + 8) * 0, 52 + 22 + (74 + 4) * 2, 122, 74, m_DialSevenBtn);
    m_DialSevenBtn->setVisible(true);
    QObject::connect(m_DialSevenBtn, ARKSENDER(released()),
                     q,              ARKRECEIVER(onToolButtonRelease()),
                     type);

    m_DialEightBtn = new BmpButton(q);
    m_DialEightBtn->setObjectName(QString("8"));
    m_DialEightBtn->setNormalBmpPath(QString(":/Images/BluetoothDialEightNormal.png"));
    m_DialEightBtn->setPressBmpPath(QString(":/Images/BluetoothDialEightPress.png"));
    g_Widget->geometryFit(30 + (122 + 8) * 1, 52 + 22 + (74 + 4) * 2, 122, 74, m_DialEightBtn);
    m_DialEightBtn->setVisible(true);
    QObject::connect(m_DialEightBtn, ARKSENDER(released()),
                     q,              ARKRECEIVER(onToolButtonRelease()),
                     type);

    m_DialNineBtn = new BmpButton(q);
    m_DialNineBtn->setObjectName(QString("9"));
    m_DialNineBtn->setNormalBmpPath(QString(":/Images/BluetoothDialNineNormal.png"));
    m_DialNineBtn->setPressBmpPath(QString(":/Images/BluetoothDialNinePress.png"));
    g_Widget->geometryFit(30 + (122 + 8) * 2, 52 + 22 + (74 + 4) * 2, 122, 74, m_DialNineBtn);
    m_DialNineBtn->setVisible(true);
    QObject::connect(m_DialNineBtn, ARKSENDER(released()),
                     q,             ARKRECEIVER(onToolButtonRelease()),
                     type);

    m_DialAsteriskBtn = new BmpButton(q);
    m_DialAsteriskBtn->setObjectName(QString("#"));
    m_DialAsteriskBtn->setNormalBmpPath(QString(":/Images/BluetoothDialPoundNormal.png"));
    m_DialAsteriskBtn->setPressBmpPath(QString(":/Images/BluetoothDialPoundPress.png"));
    g_Widget->geometryFit(30 + (122 + 8) * 0, 52 + 22 + (74 + 4) * 3, 122, 74, m_DialAsteriskBtn);
    m_DialAsteriskBtn->setVisible(true);
    QObject::connect(m_DialAsteriskBtn, ARKSENDER(released()),
                     q,                 ARKRECEIVER(onToolButtonRelease()),
                     type);

    m_DialZeroBtn = new BmpButton(q);
    m_DialZeroBtn->setObjectName(QString("0"));
    m_DialZeroBtn->setNormalBmpPath(QString(":/Images/BluetoothDialZeroNormal.png"));
    m_DialZeroBtn->setPressBmpPath(QString(":/Images/BluetoothDialZeroPress.png"));
    g_Widget->geometryFit(30 + (122 + 8) * 1, 52 + 22 + (74 + 4) * 3, 122, 74, m_DialZeroBtn);
    m_DialZeroBtn->setVisible(true);
    QObject::connect(m_DialZeroBtn, ARKSENDER(released()),
                     q,             ARKRECEIVER(onToolButtonRelease()),
                     type);

    m_DialPoundBtn = new BmpButton(q);
    m_DialPoundBtn->setObjectName(QString("*"));
    m_DialPoundBtn->setNormalBmpPath(QString(":/Images/BluetoothDialAsteriskNormal.png"));
    m_DialPoundBtn->setPressBmpPath(QString(":/Images/BluetoothDialAsteriskPress.png"));
    g_Widget->geometryFit(30 + (122 + 8) * 2, 52 + 22 + (74 + 4) * 3, 122, 74, m_DialPoundBtn);
    m_DialPoundBtn->setVisible(true);
    QObject::connect(m_DialPoundBtn, ARKSENDER(released()),
                     q,              ARKRECEIVER(onToolButtonRelease()),
                     type);
}

void BluetoothDialWidgetPrivate::connectAllSlots()
{
    Q_Q(BluetoothDialWidget);
    q->setVisible(false);
    connectSignalAndSlotByNamesake(g_Widget, q, ARKRECEIVER(onWidgetTypeChange(const Widget::Type, const Widget::Type, const QString &)));
    connectSignalAndSlotByNamesake(g_Bluetooth, q, ARKRECEIVER(onConnectStatusChange(const BluetoothConnectStatus)));
    connectSignalAndSlotByNamesake(g_Bluetooth, q, ARKRECEIVER(onVoiceChange(const BluetoothVoiceMode)));
    connectSignalAndSlotByNamesake(g_Bluetooth, q, ARKRECEIVER(onDialInfo(const BluetoothRecordType, const QString&, const QString&)));
}

