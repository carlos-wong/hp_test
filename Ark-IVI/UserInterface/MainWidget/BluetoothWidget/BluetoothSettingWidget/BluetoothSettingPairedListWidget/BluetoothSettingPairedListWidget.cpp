#include "BluetoothSettingPairedListWidget.h"
#include "AutoConnect.h"
#include "BusinessLogic/Widget.h"
#include "BmpWidget.h"
#include "BluetoothSettingPairedListItem/BluetoothSettingPairedListItem.h"
#include "AutoConnect.h"
#include "BmpWidget.h"
#include <QDebug>
#include <QPainter>
#include <QFontMetrics>

class ConnectingWidget : public QWidget
{
    Q_DISABLE_COPY(ConnectingWidget)
public:
    explicit ConnectingWidget(QWidget* parent = NULL);
    ~ConnectingWidget();
protected:
    void paintEvent(QPaintEvent* event);
private:
    BmpWidget* m_Connecting;
};

class BluetoothSettingPairedListWidgetPrivate
{
    Q_DISABLE_COPY(BluetoothSettingPairedListWidgetPrivate)
public:
    explicit BluetoothSettingPairedListWidgetPrivate(BluetoothSettingPairedListWidget* parent);
    ~BluetoothSettingPairedListWidgetPrivate();
    void initializeParent();
    void initializeBasicWidget();
    void connectAllSlots();
    void initializeConnectting();
private:
    Q_DECLARE_PUBLIC(BluetoothSettingPairedListWidget)
    BluetoothSettingPairedListWidget* const q_ptr;
    BmpWidget* m_Background;
    BluetoothSettingPairedListItem* m_FirstItem;
    BluetoothSettingPairedListItem* m_SecondItem;
    BluetoothSettingPairedListItem* m_ThirdItem;
    BluetoothSettingPairedListItem* m_FourItem;
    ConnectingWidget* m_Connecting;
};

BluetoothSettingPairedListWidget::BluetoothSettingPairedListWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new BluetoothSettingPairedListWidgetPrivate(this))
{
    onPairedListChange(g_Bluetooth->getPairedList());
}

BluetoothSettingPairedListWidget::~BluetoothSettingPairedListWidget()
{
    if (NULL != d_ptr) {
        delete d_ptr;
    }
}

void BluetoothSettingPairedListWidget::setConnectStatus(const BluetoothConnectStatus status)
{
    Q_D(BluetoothSettingPairedListWidget);
    d->m_FirstItem->setDeleteVisible((BCS_Idle == status)
                                     || (BCS_Connecting == status));
}

void BluetoothSettingPairedListWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(473, 123, 300, 260, this);
}

void BluetoothSettingPairedListWidget::onConnectStatusChange(const BluetoothConnectStatus status)
{
    Q_D(BluetoothSettingPairedListWidget);
    qDebug() << __PRETTY_FUNCTION__ << __LINE__ << status;
    switch (status) {
    case BCS_Connecting: {
        d->initializeConnectting();
        d->m_Connecting->raise();
        d->m_Connecting->setVisible(true);
        break;
    }
    default: {
        if (NULL != d->m_Connecting) {
            d->m_Connecting->setVisible(false);
        }
        break;
    }
    }
}

void BluetoothSettingPairedListWidget::onPairedListChange(const QMap<unsigned short, QString>& map)
{
    int size = map.size();
    Q_D(BluetoothSettingPairedListWidget);
    if (size > 0) {
        d->m_FirstItem->setItemText(map.value(1));
        d->m_FirstItem->setDeleteVisible((BCS_Idle == g_Bluetooth->connectStatus())
                                         || (BCS_Connecting == g_Bluetooth->connectStatus()));
    }
    d->m_FirstItem->setVisible(size > 0);
    if (size > 1) {
        d->m_SecondItem->setItemText(map.value(2));
    }
    d->m_SecondItem->setVisible(size > 1);
    if (size > 2) {
        d->m_ThirdItem->setItemText(map.value(3));
    }
    d->m_ThirdItem->setVisible(size > 2);
    if (size > 3) {
        d->m_FourItem->setItemText(map.value(4));
    }
    d->m_FourItem->setVisible(size > 3);
}

void BluetoothSettingPairedListWidget::onConnectBtnRelease()
{
    Q_D(BluetoothSettingPairedListWidget);
    BluetoothSettingPairedListItem* ptr = static_cast<BluetoothSettingPairedListItem*>(sender());
    if ((BCS_Idle == g_Bluetooth->connectStatus())
            || (BCS_Connecting == g_Bluetooth->connectStatus())) {
        if (ptr == d->m_FirstItem) {
            g_Bluetooth->connectRemoteDevice(1);
        } else if (ptr == d->m_SecondItem) {
            g_Bluetooth->connectRemoteDevice(2);
        } else if (ptr == d->m_ThirdItem) {
            g_Bluetooth->connectRemoteDevice(3);
        } else if (ptr == d->m_FourItem) {
            g_Bluetooth->connectRemoteDevice(4);
        }
    } else {
        g_Bluetooth->disconnectRemoteDevice();
    }
}

void BluetoothSettingPairedListWidget::onDeleteBtnRelease()
{
    Q_D(BluetoothSettingPairedListWidget);
    BluetoothSettingPairedListItem* ptr = static_cast<BluetoothSettingPairedListItem*>(sender());
    if (ptr == d->m_FirstItem) {
        if (BCS_Idle == g_Bluetooth->connectStatus()) {
            g_Bluetooth->deleteRemoteDevice(1);
        }
    } else if (ptr == d->m_SecondItem) {
        g_Bluetooth->deleteRemoteDevice(2);
    } else if (ptr == d->m_ThirdItem) {
        g_Bluetooth->deleteRemoteDevice(3);
    } else if (ptr == d->m_FourItem) {
        g_Bluetooth->deleteRemoteDevice(4);
    }
}

BluetoothSettingPairedListWidgetPrivate::BluetoothSettingPairedListWidgetPrivate(BluetoothSettingPairedListWidget *parent)
    : q_ptr(parent)
{
    m_Background = NULL;
    m_FirstItem = NULL;
    m_SecondItem = NULL;
    m_ThirdItem = NULL;
    m_FourItem = NULL;
    m_Connecting = NULL;
    initializeParent();
    initializeBasicWidget();
    connectAllSlots();
}

BluetoothSettingPairedListWidgetPrivate::~BluetoothSettingPairedListWidgetPrivate()
{
}

void BluetoothSettingPairedListWidgetPrivate::initializeParent()
{
    Q_Q(BluetoothSettingPairedListWidget);
    q->setVisible(true);
}

void BluetoothSettingPairedListWidgetPrivate::initializeBasicWidget()
{
    Q_Q(BluetoothSettingPairedListWidget);
    m_Background = new BmpWidget(q);
    m_Background->setBackgroundBmpPath(QString(":/Images/BluetoothSettingPairedListBackground.png"));
    g_Widget->geometryFit(0, 0, 300, 260, m_Background);
    m_Background->setVisible(true);

    m_FirstItem = new BluetoothSettingPairedListItem(q);
    g_Widget->geometryFit(0, 65 * 0, 300, 65, m_FirstItem);
    m_FirstItem->setVisible(false);
    QObject::connect(m_FirstItem, ARKSENDER(connectBtnRelease()),
                     q,           ARKRECEIVER(onConnectBtnRelease()));
    QObject::connect(m_FirstItem, ARKSENDER(deleteBtnRelease()),
                     q,           ARKRECEIVER(onDeleteBtnRelease()));

    m_SecondItem = new BluetoothSettingPairedListItem(q);
    g_Widget->geometryFit(0, 65 * 1, 300, 65, m_SecondItem);
    m_SecondItem->setVisible(false);
    QObject::connect(m_SecondItem, ARKSENDER(connectBtnRelease()),
                     q,            ARKRECEIVER(onConnectBtnRelease()));
    QObject::connect(m_SecondItem, ARKSENDER(deleteBtnRelease()),
                     q,            ARKRECEIVER(onDeleteBtnRelease()));

    m_ThirdItem = new BluetoothSettingPairedListItem(q);
    g_Widget->geometryFit(0, 65 * 2, 300, 65, m_ThirdItem);
    m_ThirdItem->setVisible(false);
    QObject::connect(m_ThirdItem, ARKSENDER(connectBtnRelease()),
                     q,           ARKRECEIVER(onConnectBtnRelease()));
    QObject::connect(m_ThirdItem, ARKSENDER(deleteBtnRelease()),
                     q,           ARKRECEIVER(onDeleteBtnRelease()));

    m_FourItem = new BluetoothSettingPairedListItem(q);
    g_Widget->geometryFit(0, 65 * 3, 300, 65, m_FourItem);
    m_FourItem->setVisible(false);
    QObject::connect(m_FourItem, ARKSENDER(connectBtnRelease()),
                     q,          ARKRECEIVER(onConnectBtnRelease()));
    QObject::connect(m_FourItem, ARKSENDER(deleteBtnRelease()),
                     q,          ARKRECEIVER(onDeleteBtnRelease()));
}

void BluetoothSettingPairedListWidgetPrivate::connectAllSlots()
{
    Q_Q(BluetoothSettingPairedListWidget);
    connectSignalAndSlotByNamesake(g_Bluetooth, q, ARKRECEIVER(onConnectStatusChange(const BluetoothConnectStatus)));
    connectSignalAndSlotByNamesake(g_Bluetooth, q, ARKRECEIVER(onPairedListChange(const QMap<unsigned short, QString>&)));
}

void BluetoothSettingPairedListWidgetPrivate::initializeConnectting()
{
    if (NULL == m_Connecting) {
        Q_Q(BluetoothSettingPairedListWidget);
        m_Connecting = new ConnectingWidget(q);
        g_Widget->geometryFit(0, 0, 300, 260, m_Connecting);
        m_Connecting->setVisible(false);
    }
}

ConnectingWidget::ConnectingWidget(QWidget *parent)
    : QWidget(parent)
{
    m_Connecting = new BmpWidget(this);
    g_Widget->geometryFit(0, 0, 300, 260, m_Connecting);
    m_Connecting->setBackgroundBmpPath(QString(":/Images/ImageLoadingBackground.png"));
    m_Connecting->setVisible(true);
}

ConnectingWidget::~ConnectingWidget()
{

}

void ConnectingWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.fillRect(rect(), QColor(0, 0, 0, 80));
}
