#include "BluetoothSettingStatusWidget.h"
#include "AutoConnect.h"
#include "BusinessLogic/Widget.h"
#include "BmpWidget.h"
#include "TextWidget.h"
#include "BmpButton.h"
#include <QDebug>

namespace SourceString {
static const QString Connect(QObject::tr("Connect"));
static const QString Connecting(QObject::tr("Connecting"));
static const QString Disconnect(QObject::tr("Disconnect"));
}

class BluetoothSettingStatusWidgetPrivate
{
public:
    explicit BluetoothSettingStatusWidgetPrivate(BluetoothSettingStatusWidget* parent);
    ~BluetoothSettingStatusWidgetPrivate();
    void initializeParent();
    void initializeBasicWidget();
    void initializeConnectDevice();
    //    void initializeConnectBtn();
    void initializeDisconnectBtn();
    void connectAllSlots();
private:
    Q_DECLARE_PUBLIC(BluetoothSettingStatusWidget)
    BluetoothSettingStatusWidget* const q_ptr;
    BmpWidget* m_Background;
    BmpWidget* m_PhoneIcon;
    TextWidget* m_ConnectDevice;
    BmpButton* m_DisconnectBtn;
    //    BmpButton* m_ConnectBtn = NULL;
};

BluetoothSettingStatusWidget::BluetoothSettingStatusWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new BluetoothSettingStatusWidgetPrivate(this))
{
}

BluetoothSettingStatusWidget::~BluetoothSettingStatusWidget()
{
    if (NULL != d_ptr) {
        delete d_ptr;
    }
}

void BluetoothSettingStatusWidget::setConnectStatus(const BluetoothConnectStatus status, const QString &name)
{
    Q_D(BluetoothSettingStatusWidget);
    if ((BCS_Idle == status)
            || (BCS_Connecting == status)) {
        if (NULL != d->m_DisconnectBtn) {
            d->m_DisconnectBtn->setVisible(false);
        }
        //        d->initializeConnectBtn();
        //        d->m_ConnectBtn->setVisible(true);
    } else {
        //        if (NULL != d->m_ConnectBtn) {
        //            d->m_ConnectBtn->setVisible(false);
        //        }
        d->initializeDisconnectBtn();
        d->m_DisconnectBtn->setVisible(true);
    }
    d->initializeConnectDevice();
    QString title;
    if (BCS_Idle == status) {
        title = QObject::tr(SourceString::Disconnect.toLocal8Bit().constData());
        qDebug() << __PRETTY_FUNCTION__ << __LINE__ << name;
    } else if (BCS_Connecting == status) {
        title = QObject::tr(SourceString::Connecting.toLocal8Bit().constData()) + QString(" ") + name;
        qDebug() << __PRETTY_FUNCTION__ << __LINE__ << name;
    } else {
        title = QObject::tr(SourceString::Connect.toLocal8Bit().constData()) + QString(" ") + name;
        qDebug() << __PRETTY_FUNCTION__ << __LINE__ << name;
    }
    d->m_ConnectDevice->setText(title);
}

void BluetoothSettingStatusWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(29, 55, 744, 60, this);
}

void BluetoothSettingStatusWidget::onToolButtonRelease()
{
    BmpButton* ptr = static_cast<BmpButton*>(sender());
    Q_D(BluetoothSettingStatusWidget);
    if (ptr == d->m_DisconnectBtn) {
        g_Bluetooth->disconnectRemoteDevice();
    }/* else if (ptr == d->m_ConnectBtn) {
        g_Bluetooth->connectRemoteDevice(0);
    }*/
}

BluetoothSettingStatusWidgetPrivate::BluetoothSettingStatusWidgetPrivate(BluetoothSettingStatusWidget *parent)
    : q_ptr(parent)
{
    m_Background = NULL;
    m_PhoneIcon = NULL;
    m_ConnectDevice = NULL;
    m_DisconnectBtn = NULL;
    //        m_ConnectBtn = NULL;
    initializeParent();
    initializeBasicWidget();
    connectAllSlots();
}

BluetoothSettingStatusWidgetPrivate::~BluetoothSettingStatusWidgetPrivate()
{
}

void BluetoothSettingStatusWidgetPrivate::initializeParent()
{
    Q_Q(BluetoothSettingStatusWidget);
    q->setVisible(true);
}

void BluetoothSettingStatusWidgetPrivate::initializeBasicWidget()
{
    Q_Q(BluetoothSettingStatusWidget);
    m_Background = new BmpWidget(q);
    m_Background->setBackgroundBmpPath(QString(":/Images/BluetoothSettingStatusBackground.png"));
    g_Widget->geometryFit(0, 0, 744, 60, m_Background);
    m_Background->setVisible(true);

    m_PhoneIcon = new BmpWidget(q);
    m_PhoneIcon->setBackgroundBmpPath(QString(":/Images/BluetoothSettingPhoneIcon.png"));
    g_Widget->geometryFit(32, 7, 26, 46, m_PhoneIcon);
    m_PhoneIcon->setVisible(true);
}

void BluetoothSettingStatusWidgetPrivate::initializeConnectDevice()
{
    if (NULL == m_ConnectDevice) {
        Q_Q(BluetoothSettingStatusWidget);
        m_ConnectDevice = new TextWidget(q);
        m_ConnectDevice->setAlignmentFlag(Qt::AlignLeft | Qt::AlignVCenter);
        m_ConnectDevice->setLanguageType(TextWidget::T_NoTranslate);
        g_Widget->geometryFit(112, 7, 324, 46, m_ConnectDevice);
        m_ConnectDevice->setVisible(true);
    }
}

//void BluetoothSettingStatusWidgetPrivate::initializeConnectBtn()
//{
//    if (NULL == m_ConnectBtn) {
//        Q_Q(BluetoothSettingStatusWidget);
//        m_ConnectBtn = new BmpButton(q);
//        m_ConnectBtn->setNormalBmpPath(QString(":/Images/BluetoothSettingConnectNormal.png"));
//        m_ConnectBtn->setPressBmpPath(QString(":/Images/BluetoothSettingConnectPress.png"));
//        g_Widget->geometryFit(623, 10, 88, 46, m_ConnectBtn);
//        QObject::connect(m_ConnectBtn, ARKSENDER(released()),
//                         q,            ARKRECEIVER(onToolButtonRelease()));
//        m_ConnectBtn->setVisible(true);
//    }
//}

void BluetoothSettingStatusWidgetPrivate::initializeDisconnectBtn()
{
    if (NULL == m_DisconnectBtn) {
        Q_Q(BluetoothSettingStatusWidget);
        m_DisconnectBtn = new BmpButton(q);
        m_DisconnectBtn->setNormalBmpPath(QString(":/Images/BluetoothSettingDisconnectNormal.png"));
        m_DisconnectBtn->setPressBmpPath(QString(":/Images/BluetoothSettingDisconnectPress.png"));
        g_Widget->geometryFit(623, 10, 88, 46, m_DisconnectBtn);
        QObject::connect(m_DisconnectBtn, ARKSENDER(released()),
                         q,               ARKRECEIVER(onToolButtonRelease()));
        m_DisconnectBtn->setVisible(true);
    }
}

void BluetoothSettingStatusWidgetPrivate::connectAllSlots()
{
}
