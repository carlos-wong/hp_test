#include "BluetoothToolWidget.h"
#include "AutoConnect.h"
#include "BmpWidget.h"
#include "BmpButton.h"
#include "BusinessLogic/Link.h"
#include <QDebug>

class BluetoothToolWidgetPrivate
{
public:
    explicit BluetoothToolWidgetPrivate(BluetoothToolWidget* parent);
    ~BluetoothToolWidgetPrivate();
    void initializeParent();
    void initializeBasicWidget();
    void connectAllSlots();
    void setDisabled(const bool flag);
private:
    Q_DECLARE_PUBLIC(BluetoothToolWidget)
    BluetoothToolWidget* const q_ptr;
    BmpWidget* m_Background;
    BmpButton* m_DialBtn;
    BmpButton* m_ContactBtn;
    BmpButton* m_RecordBtn;
    BmpButton* m_MusicBtn;
    BmpButton* m_SettingBtn;
};

BluetoothToolWidget::BluetoothToolWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new BluetoothToolWidgetPrivate(this))
{
}

BluetoothToolWidget::~BluetoothToolWidget()
{
    if (NULL != d_ptr) {
        delete d_ptr;
    }
}

void BluetoothToolWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(0, 387, 800, 94, this);
}

void BluetoothToolWidget::onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status)
{
    Q_D(BluetoothToolWidget);
    switch (destinationType) {
    case Widget::T_BluetoothDial: {
        if (WidgetStatus::Show == status) {
            d->m_DialBtn->setStatus(BmpButton::B_Check);
            d->m_ContactBtn->setStatus(BmpButton::B_Normal);
            d->m_RecordBtn->setStatus(BmpButton::B_Normal);
            d->m_MusicBtn->setStatus(BmpButton::B_Normal);
            d->m_SettingBtn->setStatus(BmpButton::B_Normal);
            setVisible(true);
        }
        break;
    }
    case Widget::T_BluetoothContact: {
        if (WidgetStatus::Show == status) {
            d->m_DialBtn->setStatus(BmpButton::B_Normal);
            d->m_ContactBtn->setStatus(BmpButton::B_Check);
            d->m_RecordBtn->setStatus(BmpButton::B_Normal);
            d->m_MusicBtn->setStatus(BmpButton::B_Normal);
            d->m_SettingBtn->setStatus(BmpButton::B_Normal);
            setVisible(true);
        }
        break;
    }
    case Widget::T_BluetoothRecord: {
        if (WidgetStatus::Show == status) {
            d->m_DialBtn->setStatus(BmpButton::B_Normal);
            d->m_ContactBtn->setStatus(BmpButton::B_Normal);
            d->m_RecordBtn->setStatus(BmpButton::B_Check);
            d->m_MusicBtn->setStatus(BmpButton::B_Normal);
            d->m_SettingBtn->setStatus(BmpButton::B_Normal);
            setVisible(true);
        }
        break;
    }
    case Widget::T_BluetoothMusic: {
        if (WidgetStatus::Show == status) {
            d->m_DialBtn->setStatus(BmpButton::B_Normal);
            d->m_ContactBtn->setStatus(BmpButton::B_Normal);
            d->m_RecordBtn->setStatus(BmpButton::B_Normal);
            d->m_MusicBtn->setStatus(BmpButton::B_Check);
            d->m_SettingBtn->setStatus(BmpButton::B_Normal);
            setVisible(true);
        }
        break;
    }
    case Widget::T_BluetoothSetting: {
        if (WidgetStatus::Show == status) {
            d->m_DialBtn->setStatus(BmpButton::B_Normal);
            d->m_ContactBtn->setStatus(BmpButton::B_Normal);
            d->m_RecordBtn->setStatus(BmpButton::B_Normal);
            d->m_MusicBtn->setStatus(BmpButton::B_Normal);
            d->m_SettingBtn->setStatus(BmpButton::B_Check);
            d->setDisabled((BCS_Idle == g_Bluetooth->connectStatus())
                           || (BCS_Connecting == g_Bluetooth->connectStatus()));
            setVisible(true);
        }
        break;
    }
    default: {
        break;
    }
    }
}

void BluetoothToolWidget::onConnectStatusChange(const BluetoothConnectStatus status)
{
    qDebug() << __PRETTY_FUNCTION__ << status;
    Q_D(BluetoothToolWidget);
    switch (status) {
    case BCS_Idle:
    case BCS_Connecting: {
        d->m_DialBtn->setStatus(BmpButton::B_Normal);
        d->m_DialBtn->setDisabled(true);
        d->m_ContactBtn->setStatus(BmpButton::B_Normal);
        d->m_ContactBtn->setDisabled(true);
        d->m_RecordBtn->setStatus(BmpButton::B_Normal);
        d->m_RecordBtn->setDisabled(true);
        d->m_MusicBtn->setStatus(BmpButton::B_Normal);
        d->m_MusicBtn->setDisabled(true);
        d->m_SettingBtn->setStatus(BmpButton::B_Check);
        if (!isVisible()) {
            setVisible(true);
        }
        d->setDisabled(true);
        d->m_SettingBtn->setDisabled(true);
        break;
    }
    case BCS_Connected: {
        d->m_DialBtn->setDisabled(false);
        d->m_ContactBtn->setDisabled(false);
        d->m_RecordBtn->setDisabled(false);
        d->m_MusicBtn->setDisabled(false);
        d->m_SettingBtn->setDisabled(false);
        break;
    }
    case BCS_Incoming:
    case BCS_Outgoing:
    case BCS_Talking: {
        g_Widget->setWidgetType(Widget::T_BluetoothDial, Widget::T_Bluetooth, WidgetStatus::RequestShow);
        d->setDisabled(true);
        d->m_SettingBtn->setDisabled(true);
        break;
    }
    default: {
        d->setDisabled(false);
        d->m_SettingBtn->setDisabled(false);
        break;
    }
    }
}

void BluetoothToolWidget::onToolButtonRelease()
{
    qDebug() << __PRETTY_FUNCTION__ << g_Bluetooth->connectStatus();
    Q_D(BluetoothToolWidget);
    bool canSwitch(true);
    switch (g_Link->getLinkType()) {
    case CARPLAY:
    case CARLIFE:
    case AUTO: {
        switch (g_Link->getLinkStatus()) {
        case LINK_STARTING:
        case LINK_SUCCESS:
        case LINK_EXITING:
        case LINK_EXITED: {
            canSwitch = false;
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
    if ((BCS_Connected == g_Bluetooth->connectStatus())
            && canSwitch) {
        BmpButton* ptr = static_cast<BmpButton*>(sender());
        if (ptr == d->m_DialBtn) {
            g_Widget->setWidgetType(Widget::T_BluetoothDial, Widget::T_Bluetooth, WidgetStatus::RequestShow);
        } else if (ptr == d->m_ContactBtn) {
            g_Widget->setWidgetType(Widget::T_BluetoothContact, Widget::T_Bluetooth, WidgetStatus::RequestShow);
        } else if (ptr == d->m_RecordBtn) {
            g_Widget->setWidgetType(Widget::T_BluetoothRecord, Widget::T_Bluetooth, WidgetStatus::RequestShow);
        } else if (ptr == d->m_MusicBtn) {
            g_Widget->setWidgetType(Widget::T_BluetoothMusic, Widget::T_Bluetooth, WidgetStatus::RequestShow);
        } else if (ptr == d->m_SettingBtn) {
            g_Widget->setWidgetType(Widget::T_BluetoothSetting, Widget::T_Bluetooth, WidgetStatus::RequestShow);
        }
    } else {
        d->m_SettingBtn->setStatus(BmpButton::B_Check);
    }
}

BluetoothToolWidgetPrivate::BluetoothToolWidgetPrivate(BluetoothToolWidget *parent)
    : q_ptr(parent)
{
    m_Background = NULL;
    m_DialBtn = NULL;
    m_ContactBtn = NULL;
    m_RecordBtn = NULL;
    m_MusicBtn = NULL;
    m_SettingBtn = NULL;
    initializeBasicWidget();
    initializeParent();
    connectAllSlots();
}

BluetoothToolWidgetPrivate::~BluetoothToolWidgetPrivate()
{
}

void BluetoothToolWidgetPrivate::initializeParent()
{
    Q_Q(BluetoothToolWidget);
    q->setVisible(false);
}

void BluetoothToolWidgetPrivate::initializeBasicWidget()
{
    Q_Q(BluetoothToolWidget);
    if (NULL == m_Background) {
        m_Background = new BmpWidget(q);
        m_Background->setBackgroundBmpPath(QString(":/Images/MultimediaToolWidgetBackground.png"));
        g_Widget->geometryFit(0, 0, 800, 94, m_Background);
        m_Background->setVisible(true);
    }
    Qt::ConnectionType type = static_cast<Qt::ConnectionType>(Qt::UniqueConnection | Qt::AutoConnection);
    if (NULL == m_DialBtn) {
        m_DialBtn = new BmpButton(q);
        m_DialBtn->setNormalBmpPath(QString(":/Images/BluetoothToolDialNormal.png"));
        m_DialBtn->setPressBmpPath(QString(":/Images/BluetoothToolDialNPress.png"));
        m_DialBtn->setCheckBmpPath(QString(":/Images/BluetoothToolDialNPress.png"));
        g_Widget->geometryFit(0, 14, 160, 80, m_DialBtn);
        QObject::connect(m_DialBtn, ARKSENDER(released()),
                         q,         ARKRECEIVER(onToolButtonRelease()),
                         type);
        m_DialBtn->setVisible(true);
    }
    if (NULL == m_ContactBtn) {
        m_ContactBtn = new BmpButton(q);
        m_ContactBtn->setNormalBmpPath(QString(":/Images/BluetoothToolContactNormal.png"));
        m_ContactBtn->setPressBmpPath(QString(":/Images/BluetoothToolContactPress.png"));
        m_ContactBtn->setCheckBmpPath(QString(":/Images/BluetoothToolContactPress.png"));
        g_Widget->geometryFit(160, 14, 160, 80, m_ContactBtn);
        QObject::connect(m_ContactBtn, ARKSENDER(released()),
                         q,            ARKRECEIVER(onToolButtonRelease()),
                         type);
        m_ContactBtn->setVisible(true);
    }
    if (NULL == m_RecordBtn) {
        m_RecordBtn = new BmpButton(q);
        m_RecordBtn->setNormalBmpPath(QString(":/Images/BluetoothToolRecordNormal.png"));
        m_RecordBtn->setPressBmpPath(QString(":/Images/BluetoothToolRecordPress.png"));
        m_RecordBtn->setCheckBmpPath(QString(":/Images/BluetoothToolRecordPress.png"));
        g_Widget->geometryFit(320, 14, 160, 80, m_RecordBtn);
        QObject::connect(m_RecordBtn, ARKSENDER(released()),
                         q,           ARKRECEIVER(onToolButtonRelease()),
                         type);
        m_RecordBtn->setVisible(true);
    }
    if (NULL == m_MusicBtn) {
        m_MusicBtn = new BmpButton(q);
        m_MusicBtn->setNormalBmpPath(QString(":/Images/BluetoothToolMusicNormal.png"));
        m_MusicBtn->setPressBmpPath(QString(":/Images/BluetoothToolMusicPress.png"));
        m_MusicBtn->setCheckBmpPath(QString(":/Images/BluetoothToolMusicPress.png"));
        g_Widget->geometryFit(480, 14, 160, 80, m_MusicBtn);
        QObject::connect(m_MusicBtn, ARKSENDER(released()),
                         q,          ARKRECEIVER(onToolButtonRelease()),
                         type);
        m_MusicBtn->setVisible(true);
        setDisabled(true);
    }
    if (NULL == m_SettingBtn) {
        m_SettingBtn = new BmpButton(q);
        m_SettingBtn->setNormalBmpPath(QString(":/Images/BluetoothToolSettingNormal.png"));
        m_SettingBtn->setPressBmpPath(QString(":/Images/BluetoothToolSettingPress.png"));
        m_SettingBtn->setCheckBmpPath(QString(":/Images/BluetoothToolSettingPress.png"));
        g_Widget->geometryFit(640, 14, 160, 80, m_SettingBtn);
        QObject::connect(m_SettingBtn, ARKSENDER(released()),
                         q,            ARKRECEIVER(onToolButtonRelease()),
                         type);
        m_SettingBtn->setVisible(true);
    }
}

void BluetoothToolWidgetPrivate::connectAllSlots()
{
    Q_Q(BluetoothToolWidget);
    connectSignalAndSlotByNamesake(g_Widget, q, ARKRECEIVER(onWidgetTypeChange(const Widget::Type, const Widget::Type, const QString &)));
    connectSignalAndSlotByNamesake(g_Bluetooth, q, ARKRECEIVER(onConnectStatusChange(const BluetoothConnectStatus)));
}

void BluetoothToolWidgetPrivate::setDisabled(const bool flag)
{
    m_DialBtn->setDisabled(flag);
    m_ContactBtn->setDisabled(flag);
    m_RecordBtn->setDisabled(flag);
    m_MusicBtn->setDisabled(flag);
}

