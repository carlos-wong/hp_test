#include "BluetoothRecordWidget.h"
#include "AutoConnect.h"
#include "BmpWidget.h"
#include "BmpButton.h"
#include "CustomListView.h"
#include "CustomItemDelegate.h"
#include "CustomScrollBar.h"
#include "../BluetoothMessageDialog.h"
#include <QStandardItemModel>
#include <QDebug>
#include <QStyleOptionViewItem>
#include <QModelIndex>
#include <QPainter>
#include <QScopedPointer>
#include <QFile>

namespace SourceString {
static const QString Clear_Sync_Outgoing(QObject::tr("Clear List, Sync Outgoing History?"));
static const QString Clear_Sync_Incoming(QObject::tr("Clear List, Sync Incoming History?"));
static const QString Clear_Sync_Missed(QObject::tr("Clear List, Sync Missed History?"));
static const QString Cancel_Sync_Other(QObject::tr("Cancel Sync Other?"));
static const QString Wait_For_Accept(QObject::tr("Wait For Accept, Make Sure Phone Permission..."));
static const QString Downloading(QObject::tr("Downloading"));
static const QString Cancelling(QObject::tr("Cancelling"));
static const QString Confirm(QObject::tr("Confirm"));
static const QString Cancel(QObject::tr("Cancel"));
}

class BluetoothRecordDelegate : public CustomItemDelegate
{
    Q_DISABLE_COPY(BluetoothRecordDelegate)
public:
    explicit BluetoothRecordDelegate(QObject* parent = NULL);
    ~BluetoothRecordDelegate();
protected:
    void paint(QPainter* painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const;
protected slots:
    void onPressIndexChanged(const QModelIndex &index);
private:
    QModelIndex m_PressIndex;
    QScopedPointer<QPixmap> m_Rules;
    QScopedPointer<QPixmap> m_Icon;
    QScopedPointer<QPixmap> m_Press;
};

class BluetoothRecordWidgetPrivate
{
public:
    explicit BluetoothRecordWidgetPrivate(BluetoothRecordWidget* parent);
    ~BluetoothRecordWidgetPrivate();
    void initializeParent();
    void initializeBasicWidget();
    void initializeMessageDialog();
    void initializeOutgoingWidget();
    void initializeIncomingWidget();
    void initializeMissedWidget();
    void connectAllSlots();
private:
    Q_DECLARE_PUBLIC(BluetoothRecordWidget)
    BluetoothRecordWidget* const q_ptr;
    BmpWidget* m_Background;
    BluetoothMessageDialog* m_MessageDialog;
    BmpButton* m_OutgoingBtn;
    BmpButton* m_IncomingBtn;
    BmpButton* m_MissedBtn;
    BmpButton* m_SyncBtn;
    CustomListView* m_OutgoingCustomListView;
    QStandardItemModel* m_OutgoingStandardItemModel;
    BluetoothRecordDelegate* m_OutgoingDelegate;
    CustomScrollBar* m_OutgoingCustomScrollBar;

    CustomListView* m_IncomingCustomListView;
    QStandardItemModel* m_IncomingStandardItemModel;
    BluetoothRecordDelegate* m_IncomingDelegate;
    CustomScrollBar* m_IncomingCustomScrollBar;

    CustomListView* m_MissedCustomListView;
    QStandardItemModel* m_MissedStandardItemModel;
    BluetoothRecordDelegate* m_MissedDelegate;
    CustomScrollBar* m_MissedCustomScrollBar;
};

BluetoothRecordWidget::BluetoothRecordWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new BluetoothRecordWidgetPrivate(this))
{
}

BluetoothRecordWidget::~BluetoothRecordWidget()
{
    if (NULL != d_ptr) {
        delete d_ptr;
    }
}

void BluetoothRecordWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(0, 0, 800, 480, this);
}

void BluetoothRecordWidget::onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status)
{
    switch (destinationType) {
    case Widget::T_BluetoothContact: {
        if (WidgetStatus::Show == status) {
            Q_D(BluetoothRecordWidget);
            if (NULL != d->m_MessageDialog) {
                if ((QObject::tr(SourceString::Clear_Sync_Outgoing.toLocal8Bit().constData()) == d->m_MessageDialog->getTitle())
                        || (QObject::tr(SourceString::Clear_Sync_Incoming.toLocal8Bit().constData()) == d->m_MessageDialog->getTitle())
                        || (QObject::tr(SourceString::Clear_Sync_Missed.toLocal8Bit().constData()) == d->m_MessageDialog->getTitle())
                        || (QObject::tr(SourceString::Cancel_Sync_Other.toLocal8Bit().constData()) == d->m_MessageDialog->getTitle())) {
                    d->m_MessageDialog->setVisible(false);
                }
            }
            setVisible(false);
        }
    }
    case Widget::T_BluetoothDial:
    case Widget::T_BluetoothMusic:
    case Widget::T_BluetoothSetting: {
        if (WidgetStatus::Show == status) {
            setVisible(false);
        }
        break;
    }
    case Widget::T_BluetoothRecord: {
        if (WidgetStatus::RequestShow == status) {
            g_Widget->setWidgetType(Widget::T_BluetoothRecord, requestType, WidgetStatus::Show);
        } else if (WidgetStatus::Show == status) {
            lower();
            Q_D(BluetoothRecordWidget);
            d->initializeOutgoingWidget();
            if (!isVisible()) {
                setVisible(true);
            }
        }
        break;
    }
    default: {
        break;
    }
    }
}

void BluetoothRecordWidget::onConnectStatusChange(const BluetoothConnectStatus status)
{
    if ((BCS_Idle == status)
            || (BCS_Connecting == status)) {
        Q_D(BluetoothRecordWidget);
        if (NULL != d->m_OutgoingStandardItemModel) {
            d->m_OutgoingStandardItemModel->clear();
        }
        if (NULL != d->m_OutgoingCustomScrollBar) {
            d->m_OutgoingCustomScrollBar->setValue(0);
        }
        if (NULL != d->m_IncomingStandardItemModel) {
            d->m_IncomingStandardItemModel->clear();
        }
        if (NULL != d->m_IncomingCustomScrollBar) {
            d->m_IncomingCustomScrollBar->setValue(0);
        }
        if (NULL != d->m_MissedStandardItemModel) {
            d->m_MissedStandardItemModel->clear();
        }
        if (NULL != d->m_MissedCustomScrollBar) {
            d->m_MissedCustomScrollBar->setValue(0);
        }
        if (NULL != d->m_MessageDialog) {
            d->m_MessageDialog->setConfirmVisible(true);
            d->m_MessageDialog->setVisible(false);
        }
        setVisible(false);
    }
}

void BluetoothRecordWidget::onRecordChange(const BluetoothRecordType type, const QList<ContactInfo> &list)
{
    Q_D(BluetoothRecordWidget);
    switch (type) {
    case BRT_PhoneBook: {
        d->m_OutgoingBtn->setDisabled(false);
        d->m_IncomingBtn->setDisabled(false);
        d->m_MissedBtn->setDisabled(false);
        d->m_SyncBtn->setDisabled(false);
        if (NULL != d->m_MessageDialog) {
            d->m_MessageDialog->setVisible(false);
            d->m_MessageDialog->setTitle(QObject::tr(SourceString::Clear_Sync_Outgoing.toLocal8Bit().constData()));
            d->m_MessageDialog->setConfirmVisible(true);
        }
        break;
    }
    case BRT_Outgoing: {
        if (NULL != d->m_OutgoingCustomListView) {
            d->m_OutgoingStandardItemModel->clear();
            d->m_OutgoingCustomScrollBar->setValue(0);
            d->m_OutgoingCustomListView->update();
            QStandardItem* root = d->m_OutgoingStandardItemModel->invisibleRootItem();
            for (int i = 0; i < list.size(); ++i) {
                QStandardItem* item = new QStandardItem();
                item->setSizeHint(QSize((596)  * g_Widget->widthScalabilityFactor(), 55 * g_Widget->heightScalabilityFactor()));
                ContactInfo variant;
                variant.name = list.at(i).name;
                variant.phone = list.at(i).phone;
                item->setData(qVariantFromValue(variant), Qt::DisplayRole);
                root->setChild(root->rowCount(), 0, item);
            }
            d->m_OutgoingBtn->setDisabled(false);
            d->m_IncomingBtn->setDisabled(false);
            d->m_MissedBtn->setDisabled(false);
            d->m_SyncBtn->setDisabled(false);
            if (NULL != d->m_MessageDialog) {
                d->m_MessageDialog->setVisible(false);
                d->m_MessageDialog->setTitle(QObject::tr(SourceString::Clear_Sync_Outgoing.toLocal8Bit().constData()));
                d->m_MessageDialog->setConfirmVisible(true);
            }
        } else {
            d->initializeOutgoingWidget();
        }
        break;
    }
    case BRT_Incoming: {
        if (NULL != d->m_IncomingCustomListView) {
            d->m_IncomingStandardItemModel->clear();
            d->m_IncomingCustomScrollBar->setValue(0);
            d->m_IncomingCustomListView->update();
            QStandardItem* root = d->m_IncomingStandardItemModel->invisibleRootItem();
            for (int i = 0; i < list.size(); ++i) {
                QStandardItem* item = new QStandardItem();
                item->setSizeHint(QSize((596)  * g_Widget->widthScalabilityFactor(), 55 * g_Widget->heightScalabilityFactor()));
                ContactInfo variant;
                variant.name = list.at(i).name;
                variant.phone = list.at(i).phone;
                item->setData(qVariantFromValue(variant), Qt::DisplayRole);
                root->setChild(root->rowCount(), 0, item);
            }
            d->m_OutgoingBtn->setDisabled(false);
            d->m_IncomingBtn->setDisabled(false);
            d->m_MissedBtn->setDisabled(false);
            d->m_SyncBtn->setDisabled(false);
            if (NULL != d->m_MessageDialog) {
                d->m_MessageDialog->setVisible(false);
                d->m_MessageDialog->setTitle(QObject::tr(SourceString::Clear_Sync_Incoming.toLocal8Bit().constData()));
                d->m_MessageDialog->setConfirmVisible(true);
            }
        } else {
            d->initializeIncomingWidget();
        }
        break;
    }
    case BRT_Missed: {
        qDebug() << __PRETTY_FUNCTION__ << __LINE__ << list.size();
        if (NULL != d->m_MissedCustomListView) {
            qDebug() << __PRETTY_FUNCTION__ << __LINE__;
            d->m_MissedStandardItemModel->clear();
            d->m_MissedCustomScrollBar->setValue(0);
            d->m_MissedCustomListView->update();
            QStandardItem* root = d->m_MissedStandardItemModel->invisibleRootItem();
            for (int i = 0; i < list.size(); ++i) {
                QStandardItem* item = new QStandardItem();
                item->setSizeHint(QSize((596)  * g_Widget->widthScalabilityFactor(), 55 * g_Widget->heightScalabilityFactor()));
                ContactInfo variant;
                variant.name = list.at(i).name;
                variant.phone = list.at(i).phone;
                item->setData(qVariantFromValue(variant), Qt::DisplayRole);
                root->setChild(root->rowCount(), 0, item);
            }
            d->m_OutgoingBtn->setDisabled(false);
            d->m_IncomingBtn->setDisabled(false);
            d->m_MissedBtn->setDisabled(false);
            d->m_SyncBtn->setDisabled(false);
            if (NULL != d->m_MessageDialog) {
                d->m_MessageDialog->setVisible(false);
                d->m_MessageDialog->setTitle(QObject::tr(SourceString::Clear_Sync_Missed.toLocal8Bit().constData()));
                d->m_MessageDialog->setConfirmVisible(true);
            }
        } else {
            qDebug() << __PRETTY_FUNCTION__ << __LINE__;
            d->initializeMissedWidget();
        }
        break;
    }
    default: {
        break;
    }
    }
}

void BluetoothRecordWidget::onRecordCountChange(const BluetoothRecordType type, const unsigned count)
{
    Q_D(BluetoothRecordWidget);
    switch (type) {
    case BRT_Outgoing: {
        if ((NULL != d->m_OutgoingCustomListView)
                && (d->m_OutgoingCustomListView->isVisible())) {
            if (d->m_MessageDialog->isVisible()) {
                d->m_MessageDialog->setTitle(QObject::tr(SourceString::Downloading.toLocal8Bit().constData()) + QString("(") + QString::number(count) + QString(")"));
            }
        }
        break;
    }
    case BRT_Incoming: {
        if ((NULL != d->m_IncomingCustomListView)
                && (d->m_IncomingCustomListView->isVisible())) {
            if (d->m_MessageDialog->isVisible()) {
                d->m_MessageDialog->setTitle(QObject::tr(SourceString::Downloading.toLocal8Bit().constData()) + QString("(") + QString::number(count) + QString(")"));
            }
        }
        break;
    }
    case BRT_Missed: {
        if ((NULL != d->m_MissedCustomListView)
                && (d->m_MissedCustomListView->isVisible())){
            if (d->m_MessageDialog->isVisible()) {
                d->m_MessageDialog->setTitle(QObject::tr(SourceString::Downloading.toLocal8Bit().constData()) + QString("(") + QString::number(count) + QString(")"));
            }
        }
        break;
    }
    default: {
        return;
        break;
    }
    }
}

void BluetoothRecordWidget::onSyncChange(const BluetoothRecordType type, const BluetoothSyncStatus status)
{
    Q_D(BluetoothRecordWidget);
    switch (status) {
    case BSS_Undefine: {
        d->initializeMessageDialog();
        d->m_MessageDialog->setVisible(false);
        d->m_MessageDialog->setConfirmVisible(true);
        d->m_OutgoingBtn->setDisabled(false);
        d->m_IncomingBtn->setDisabled(false);
        d->m_MissedBtn->setDisabled(false);
        d->m_SyncBtn->setDisabled(false);
        break;
    }
    case BSS_Cacelling: {
        d->m_OutgoingBtn->setDisabled(true);
        d->m_IncomingBtn->setDisabled(true);
        d->m_MissedBtn->setDisabled(true);
        d->m_SyncBtn->setDisabled(true);
        d->initializeMessageDialog();
        d->m_MessageDialog->setConfirmVisible(false);
        d->m_MessageDialog->setTitle(QObject::tr(SourceString::Cancelling.toLocal8Bit().constData()));
        d->m_MessageDialog->setVisible(true);
        break;
    }
    default: {
        break;
    }
    }
}

void BluetoothRecordWidget::onToolButtonRelease()
{
    Q_D(BluetoothRecordWidget);
    BmpButton* ptr = static_cast<BmpButton*>(sender());
    if (ptr == d->m_SyncBtn) {
        if ((NULL != d->m_OutgoingCustomListView)
                && (d->m_OutgoingCustomListView->isVisible())) {
            d->initializeMessageDialog();
            switch (g_Bluetooth->getSynchronizeType()) {
            case BRT_PhoneBook: {
                d->m_MessageDialog->setConfirmVisible(true);
                d->m_MessageDialog->setTitle(QObject::tr(SourceString::Cancel_Sync_Other.toLocal8Bit().constData()));
                break;
            }
            default: {
                d->m_MessageDialog->setConfirmVisible(true);
                d->m_MessageDialog->setTitle(QObject::tr(SourceString::Clear_Sync_Outgoing.toLocal8Bit().constData()));
                break;
            }
            }
        } else if ((NULL != d->m_IncomingCustomListView) &&
                   (d->m_IncomingCustomListView->isVisible())) {
            d->initializeMessageDialog();
            switch (g_Bluetooth->getSynchronizeType()) {
            case BRT_PhoneBook: {
                d->m_MessageDialog->setConfirmVisible(true);
                d->m_MessageDialog->setTitle(QObject::tr(SourceString::Cancel_Sync_Other.toLocal8Bit().constData()));
                break;
            }
            default: {
                d->m_MessageDialog->setConfirmVisible(true);
                d->m_MessageDialog->setTitle(QObject::tr(SourceString::Clear_Sync_Incoming.toLocal8Bit().constData()));
                break;
            }
            }
        } else {
            d->initializeMessageDialog();
            switch (g_Bluetooth->getSynchronizeType()) {
            case BRT_PhoneBook: {
                d->m_MessageDialog->setConfirmVisible(true);
                d->m_MessageDialog->setTitle(QObject::tr(SourceString::Cancel_Sync_Other.toLocal8Bit().constData()));
                break;
            }
            default: {
                d->m_MessageDialog->setConfirmVisible(true);
                d->m_MessageDialog->setTitle(QObject::tr(SourceString::Clear_Sync_Missed.toLocal8Bit().constData()));
                break;
            }
            }
        }
        d->m_MessageDialog->raise();
        d->m_MessageDialog->setConfirmVisible(true);
        d->m_MessageDialog->setVisible(true);
        return ;
    }
    if (NULL != d->m_MessageDialog) {
        d->m_MessageDialog->setVisible(false);
    }
    if (ptr == d->m_OutgoingBtn) {
        if (NULL == d->m_OutgoingCustomListView) {
            d->initializeOutgoingWidget();
        }
        d->m_OutgoingBtn->setStatus(BmpButton::B_Check);
    } else {
        d->m_OutgoingBtn->setStatus(BmpButton::B_Normal);
    }
    if (NULL != d->m_OutgoingCustomListView) {
        d->m_OutgoingCustomListView->setVisible(ptr == d->m_OutgoingBtn);
        qDebug() << "d->m_OutgoingCustomListView" << d->m_OutgoingCustomListView->isVisible();
    }
    if (ptr == d->m_IncomingBtn) {
        if (NULL == d->m_IncomingCustomListView) {
            d->initializeIncomingWidget();
        }
        d->m_IncomingBtn->setStatus(BmpButton::B_Check);
    } else {
        d->m_IncomingBtn->setStatus(BmpButton::B_Normal);
    }
    if (NULL != d->m_IncomingCustomListView) {
        d->m_IncomingCustomListView->setVisible(ptr == d->m_IncomingBtn);
        qDebug() << "d->m_IncomingCustomListView" << d->m_IncomingCustomListView->isVisible();
    }
    if (ptr == d->m_MissedBtn) {
        if (NULL == d->m_MissedCustomListView) {
            d->initializeMissedWidget();
        }
        d->m_MissedBtn->setStatus(BmpButton::B_Check);
    } else {
        d->m_MissedBtn->setStatus(BmpButton::B_Normal);
    }
    if (NULL != d->m_MissedCustomListView) {
        d->m_MissedCustomListView->setVisible(ptr == d->m_MissedBtn);
        qDebug() << "d->m_MissedCustomListView" << d->m_MissedCustomListView->isVisible();
    }
}

void BluetoothRecordWidget::onListViewItemRelease(const int index)
{
    CustomListView* ptr = static_cast<CustomListView*>(sender());
    QStandardItemModel* model = NULL;
    Q_D(BluetoothRecordWidget);
    if (ptr == d->m_OutgoingCustomListView) {
        model = d->m_OutgoingStandardItemModel;
    } else if (ptr == d->m_IncomingCustomListView) {
        model = d->m_IncomingStandardItemModel;
    } else if (ptr == d->m_MissedCustomListView) {
        model = d->m_MissedStandardItemModel;
    } else {
        return;
    }
    ContactInfo variant = qVariantValue<ContactInfo>(model->index(index, 0, QModelIndex()).data(Qt::DisplayRole));
    g_Bluetooth->dialPhone(variant.phone);
}

void BluetoothRecordWidget::onConfirm()
{
    Q_D(BluetoothRecordWidget);
    if (NULL != d->m_MessageDialog) {
        if (QObject::tr(SourceString::Cancel_Sync_Other.toLocal8Bit().constData()) == d->m_MessageDialog->getTitle()) {
            g_Bluetooth->cancelSynchronize(g_Bluetooth->getSynchronizeType());
            d->m_SyncBtn->setDisabled(false);
        } else {
            if ((NULL != d->m_OutgoingCustomListView)
                    && (d->m_OutgoingCustomListView->isVisible())) {
                d->m_OutgoingBtn->setDisabled(true);
                d->m_IncomingBtn->setDisabled(true);
                d->m_MissedBtn->setDisabled(true);
                d->m_SyncBtn->setDisabled(true);
                d->m_OutgoingStandardItemModel->clear();
                d->m_OutgoingCustomScrollBar->setValue(0);
                d->m_MessageDialog->setConfirmVisible(false);
                d->m_MessageDialog->setTitle(QObject::tr(SourceString::Wait_For_Accept.toLocal8Bit().constData()));
                g_Bluetooth->synchronizeOutgoing();
            } else if ((NULL != d->m_IncomingCustomListView)
                       && (d->m_IncomingCustomListView->isVisible())) {
                d->m_OutgoingBtn->setDisabled(true);
                d->m_IncomingBtn->setDisabled(true);
                d->m_MissedBtn->setDisabled(true);
                d->m_SyncBtn->setDisabled(true);
                d->m_IncomingStandardItemModel->clear();
                d->m_IncomingCustomScrollBar->setValue(0);
                d->m_MessageDialog->setConfirmVisible(false);
                d->m_MessageDialog->setTitle(QObject::tr(SourceString::Wait_For_Accept.toLocal8Bit().constData()));
                g_Bluetooth->synchronizeIncoming();
            } else if ((NULL != d->m_MissedCustomListView)
                       && (d->m_MissedCustomListView->isVisible())) {
                d->m_OutgoingBtn->setDisabled(true);
                d->m_IncomingBtn->setDisabled(true);
                d->m_MissedBtn->setDisabled(true);
                d->m_SyncBtn->setDisabled(true);
                d->m_MissedStandardItemModel->clear();
                d->m_MissedCustomScrollBar->setValue(0);
                d->m_MessageDialog->setConfirmVisible(false);
                d->m_MessageDialog->setTitle(QObject::tr(SourceString::Wait_For_Accept.toLocal8Bit().constData()));
                g_Bluetooth->synchronizeMissed();
            }
        }
    }
}

void BluetoothRecordWidget::onCancel()
{
    Q_D(BluetoothRecordWidget);
    if (NULL != d->m_MessageDialog) {
        if (BRT_Undefine == g_Bluetooth->getSynchronizeType()) {
            d->m_MessageDialog->setVisible(false);
        } else {
            if (BRT_PhoneBook == g_Bluetooth->getSynchronizeType()) {
                d->m_MessageDialog->setVisible(false);
            } else {
                if ((NULL != d->m_OutgoingCustomListView)
                        && (d->m_OutgoingCustomListView->isVisible())) {
                    g_Bluetooth->cancelSynchronize(BRT_Outgoing);
                } else if ((NULL != d->m_IncomingCustomListView) &&
                           (d->m_IncomingCustomListView->isVisible())) {
                    g_Bluetooth->cancelSynchronize(BRT_Incoming);
                } else if ((NULL != d->m_MissedCustomListView) &&
                           (d->m_MissedCustomListView->isVisible())) {
                    g_Bluetooth->cancelSynchronize(BRT_Missed);
                }
            }
        }
    }
}

BluetoothRecordWidgetPrivate::BluetoothRecordWidgetPrivate(BluetoothRecordWidget *parent)
    : q_ptr(parent)
{
    m_Background = NULL;
    m_MessageDialog = NULL;
    m_OutgoingBtn = NULL;
    m_IncomingBtn = NULL;
    m_MissedBtn = NULL;
    m_SyncBtn = NULL;
    m_OutgoingCustomListView = NULL;
    m_OutgoingStandardItemModel = NULL;
    m_OutgoingDelegate = NULL;
    m_OutgoingCustomScrollBar = NULL;

    m_IncomingCustomListView = NULL;
    m_IncomingStandardItemModel = NULL;
    m_IncomingDelegate = NULL;
    m_IncomingCustomScrollBar = NULL;

    m_MissedCustomListView = NULL;
    m_MissedStandardItemModel = NULL;
    m_MissedDelegate = NULL;
    m_MissedCustomScrollBar = NULL;
    initializeParent();
    initializeBasicWidget();
    connectAllSlots();
}

BluetoothRecordWidgetPrivate::~BluetoothRecordWidgetPrivate()
{
}

void BluetoothRecordWidgetPrivate::initializeParent()
{
    Q_Q(BluetoothRecordWidget);
    q->setVisible(false);
}

void BluetoothRecordWidgetPrivate::initializeBasicWidget()
{
    Q_Q(BluetoothRecordWidget);
    m_Background = new BmpWidget(q);
    m_Background->setBackgroundBmpPath(QString(":/Images/SettingWidgetBackground.png"));
    g_Widget->geometryFit(175, 54, 600, 334, m_Background);
    m_Background->setVisible(true);

    m_OutgoingBtn = new BmpButton(q);
    m_OutgoingBtn->setNormalBmpPath(QString(":/Images/BluetoothRecordOutgoingNormal.png"));
    m_OutgoingBtn->setPressBmpPath(QString(":/Images/BluetoothRecordOutgoingPress.png"));
    m_OutgoingBtn->setCheckBmpPath(QString(":/Images/BluetoothRecordOutgoingPress.png"));
    g_Widget->geometryFit(29, 54, 140, 70, m_OutgoingBtn);
    m_OutgoingBtn->setVisible(true);
    m_OutgoingBtn->setStatus(BmpButton::B_Check);
    QObject::connect(m_OutgoingBtn, ARKSENDER(released()),
                     q,             ARKRECEIVER(onToolButtonRelease()));

    m_IncomingBtn = new BmpButton(q);
    m_IncomingBtn->setNormalBmpPath(QString(":/Images/BluetoothRecordIncomingNormal.png"));
    m_IncomingBtn->setPressBmpPath(QString(":/Images/BluetoothRecordIncomingPress.png"));
    m_IncomingBtn->setCheckBmpPath(QString(":/Images/BluetoothRecordIncomingPress.png"));
    g_Widget->geometryFit(29, 139, 140, 70, m_IncomingBtn);
    m_IncomingBtn->setVisible(true);
    m_IncomingBtn->setStatus(BmpButton::B_Normal);
    QObject::connect(m_IncomingBtn, ARKSENDER(released()),
                     q,             ARKRECEIVER(onToolButtonRelease()));

    m_MissedBtn = new BmpButton(q);
    m_MissedBtn->setNormalBmpPath(QString(":/Images/BluetoothRecordMissedNormal.png"));
    m_MissedBtn->setPressBmpPath(QString(":/Images/BluetoothRecordMissedPress.png"));
    m_MissedBtn->setCheckBmpPath(QString(":/Images/BluetoothRecordMissedPress.png"));
    g_Widget->geometryFit(29, 225, 140, 70, m_MissedBtn);
    m_MissedBtn->setVisible(true);
    m_MissedBtn->setStatus(BmpButton::B_Normal);
    QObject::connect(m_MissedBtn, ARKSENDER(released()),
                     q,           ARKRECEIVER(onToolButtonRelease()));

    m_SyncBtn = new BmpButton(q);
    m_SyncBtn->setNormalBmpPath(QString(":/Images/BluetoothContactSyncNormal.png"));
    m_SyncBtn->setPressBmpPath(QString(":/Images/BluetoothContactSyncPress.png"));
    m_SyncBtn->setCheckBmpPath(QString(":/Images/BluetoothContactSyncPress.png"));
    g_Widget->geometryFit(29, 311, 140, 70, m_SyncBtn);
    m_SyncBtn->setVisible(true);
    m_SyncBtn->setStatus(BmpButton::B_Normal);
    QObject::connect(m_SyncBtn, ARKSENDER(released()),
                     q,         ARKRECEIVER(onToolButtonRelease()));

}

void BluetoothRecordWidgetPrivate::initializeMessageDialog()
{
    if (NULL == m_MessageDialog) {
        Q_Q(BluetoothRecordWidget);
        m_MessageDialog = new BluetoothMessageDialog(q);
        m_MessageDialog->setCancel(SourceString::Cancel);
        m_MessageDialog->setConfirm(SourceString::Confirm);
        m_MessageDialog->setVisible(false);
        QObject::connect(m_MessageDialog, ARKSENDER(confirm()),
                         q,               ARKRECEIVER(onConfirm()));
        QObject::connect(m_MessageDialog, ARKSENDER(cancel()),
                         q,               ARKRECEIVER(onCancel()));
    }
}

void BluetoothRecordWidgetPrivate::initializeOutgoingWidget()
{
    static bool flag(false);
    if (!flag) {
        flag = true;
        Q_Q(BluetoothRecordWidget);
        m_OutgoingCustomListView = new CustomListView(q);
        m_OutgoingCustomScrollBar = new CustomScrollBar(q);
        m_OutgoingCustomScrollBar->setStyleSheet("QScrollBar:vertical{"
                                                 "width: " + QString::number(int(47 * g_Widget->widthScalabilityFactor())) + "px;"
                                                                                                                             "background: transparent;"
                                                                                                                             "margin: 0px, 0px, 0px, 0px;"
                                                                                                                             "padding: 0px, 0px, 0px, 0px;"
                                                                                                                             "}"
                                                                                                                             "QScrollBar::handle:vertical{"
                                                                                                                             "border-top-right-radius: " + QString::number(int(10 * g_Widget->widthScalabilityFactor())) + "px;"
                                                                                                                                                                                                                           "border-bottom-right-radius: " + QString::number(int(10 * g_Widget->widthScalabilityFactor())) + "px;"
                                                                                                                                                                                                                                                                                                                            "min-height: " + QString::number(int(47 * g_Widget->heightScalabilityFactor())) + "px;"
                                                                                                                                                                                                                                                                                                                                                                                                              "background: rgba(0, 162, 255, 191);"
                                                                                                                                                                                                                                                                                                                                                                                                              "}"
                                                                                                                                                                                                                                                                                                                                                                                                              "QScrollBar::add-line:vertical{"
                                                                                                                                                                                                                                                                                                                                                                                                              "height: 0px;"
                                                                                                                                                                                                                                                                                                                                                                                                              "width: 0px;"
                                                                                                                                                                                                                                                                                                                                                                                                              "}"
                                                                                                                                                                                                                                                                                                                                                                                                              "QScrollBar::sub-line:vertical{"
                                                                                                                                                                                                                                                                                                                                                                                                              "height: 0px;"
                                                                                                                                                                                                                                                                                                                                                                                                              "width: 0px;"
                                                                                                                                                                                                                                                                                                                                                                                                              "}"
                                                                                                                                                                                                                                                                                                                                                                                                              "QScrollBar::add-page:vertical{"
                                                                                                                                                                                                                                                                                                                                                                                                              "margin-left: " + QString::number(int(18 * g_Widget->widthScalabilityFactor())) + "px;"
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                "margin-right: " + QString::number(int(18 * g_Widget->widthScalabilityFactor())) + "px;"
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   "background: rgba(42, 42, 42, 191);"
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   "}"
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   "QScrollBar::sub-page:vertical{"
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   "margin-left: " + QString::number(int(18 * g_Widget->widthScalabilityFactor())) + "px;"
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     "margin-right: " + QString::number(int(18 * g_Widget->widthScalabilityFactor())) + "px;"
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        "background: rgba(42, 42, 42, 191);"
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        "}"
                                                 );
        m_OutgoingDelegate = new BluetoothRecordDelegate(q);
        m_OutgoingStandardItemModel = new QStandardItemModel(q);
        m_OutgoingCustomListView->setVerticalScrollBar(m_OutgoingCustomScrollBar);
        m_OutgoingCustomListView->setItemDelegate(m_OutgoingDelegate);
        m_OutgoingCustomListView->setModel(m_OutgoingStandardItemModel);
        g_Widget->geometryFit(175, 54, 596, 328, m_OutgoingCustomListView);
        m_OutgoingCustomListView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        QObject::connect(m_OutgoingCustomListView, ARKSENDER(listViewItemRelease(const int)),
                         q,                        ARKRECEIVER(onListViewItemRelease(const int)));
        const QList<ContactInfo>& list = g_Bluetooth->getRecordList(BRT_Outgoing);
        QStandardItem* root = m_OutgoingStandardItemModel->invisibleRootItem();
        for (int i = 0; i < list.size(); ++i) {
            QStandardItem* item = new QStandardItem();
            item->setSizeHint(QSize((596)  * g_Widget->widthScalabilityFactor(), 55 * g_Widget->heightScalabilityFactor()));
            ContactInfo variant;
            variant.name = list.at(i).name;
            variant.phone = list.at(i).phone;
            item->setData(qVariantFromValue(variant), Qt::DisplayRole);
            root->setChild(root->rowCount(), 0, item);
        }
        m_OutgoingCustomListView->setVisible(true);
    }
}

void BluetoothRecordWidgetPrivate::initializeIncomingWidget()
{
    static bool flag(false);
    if (!flag) {
        flag = true;
        Q_Q(BluetoothRecordWidget);
        m_IncomingCustomListView = new CustomListView(q);
        m_IncomingCustomScrollBar = new CustomScrollBar(q);
        m_IncomingCustomScrollBar->setStyleSheet("QScrollBar:vertical{"
                                                 "width: " + QString::number(int(47 * g_Widget->widthScalabilityFactor())) + "px;"
                                                                                                                             "background: transparent;"
                                                                                                                             "margin: 0px, 0px, 0px, 0px;"
                                                                                                                             "padding: 0px, 0px, 0px, 0px;"
                                                                                                                             "}"
                                                                                                                             "QScrollBar::handle:vertical{"
                                                                                                                             "border-top-right-radius: " + QString::number(int(10 * g_Widget->widthScalabilityFactor())) + "px;"
                                                                                                                                                                                                                           "border-bottom-right-radius: " + QString::number(int(10 * g_Widget->widthScalabilityFactor())) + "px;"
                                                                                                                                                                                                                                                                                                                            "min-height: " + QString::number(int(47 * g_Widget->heightScalabilityFactor())) + "px;"
                                                                                                                                                                                                                                                                                                                                                                                                              "background: rgba(0, 162, 255, 191);"
                                                                                                                                                                                                                                                                                                                                                                                                              "}"
                                                                                                                                                                                                                                                                                                                                                                                                              "QScrollBar::add-line:vertical{"
                                                                                                                                                                                                                                                                                                                                                                                                              "height: 0px;"
                                                                                                                                                                                                                                                                                                                                                                                                              "width: 0px;"
                                                                                                                                                                                                                                                                                                                                                                                                              "}"
                                                                                                                                                                                                                                                                                                                                                                                                              "QScrollBar::sub-line:vertical{"
                                                                                                                                                                                                                                                                                                                                                                                                              "height: 0px;"
                                                                                                                                                                                                                                                                                                                                                                                                              "width: 0px;"
                                                                                                                                                                                                                                                                                                                                                                                                              "}"
                                                                                                                                                                                                                                                                                                                                                                                                              "QScrollBar::add-page:vertical{"
                                                                                                                                                                                                                                                                                                                                                                                                              "margin-left: " + QString::number(int(18 * g_Widget->widthScalabilityFactor())) + "px;"
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                "margin-right: " + QString::number(int(18 * g_Widget->widthScalabilityFactor())) + "px;"
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   "background: rgba(42, 42, 42, 191);"
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   "}"
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   "QScrollBar::sub-page:vertical{"
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   "margin-left: " + QString::number(int(18 * g_Widget->widthScalabilityFactor())) + "px;"
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     "margin-right: " + QString::number(int(18 * g_Widget->widthScalabilityFactor())) + "px;"
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        "background: rgba(42, 42, 42, 191);"
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        "}"
                                                 );
        m_IncomingDelegate = new BluetoothRecordDelegate(q);
        m_IncomingStandardItemModel = new QStandardItemModel(q);
        m_IncomingCustomListView->setVerticalScrollBar(m_IncomingCustomScrollBar);
        m_IncomingCustomListView->setItemDelegate(m_IncomingDelegate);
        m_IncomingCustomListView->setModel(m_IncomingStandardItemModel);
        g_Widget->geometryFit(175, 54, 596, 328, m_IncomingCustomListView);
        m_IncomingCustomListView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        QObject::connect(m_IncomingCustomListView, ARKSENDER(listViewItemRelease(const int)),
                         q,                        ARKRECEIVER(onListViewItemRelease(const int)));
        QStandardItem* root = m_IncomingStandardItemModel->invisibleRootItem();
        const QList<ContactInfo>& list = g_Bluetooth->getRecordList(BRT_Incoming);
        for (int i = 0; i < list.size(); ++i) {
            QStandardItem* item = new QStandardItem();
            item->setSizeHint(QSize((596)  * g_Widget->widthScalabilityFactor(), 55 * g_Widget->heightScalabilityFactor()));
            ContactInfo variant;
            variant.name = list.at(i).name;
            variant.phone = list.at(i).phone;
            item->setData(qVariantFromValue(variant), Qt::DisplayRole);
            root->setChild(root->rowCount(), 0, item);
        }
        m_IncomingCustomListView->setVisible(true);
    }
}

void BluetoothRecordWidgetPrivate::initializeMissedWidget()
{
    static bool flag(false);
    qDebug() << __PRETTY_FUNCTION__ << __LINE__;
    if (!flag) {
        qDebug() << __PRETTY_FUNCTION__ << __LINE__;
        flag = true;
        Q_Q(BluetoothRecordWidget);
        m_MissedCustomListView = new CustomListView(q);
        m_MissedCustomScrollBar = new CustomScrollBar(q);
        m_MissedCustomScrollBar->setStyleSheet("QScrollBar:vertical{"
                                               "width: " + QString::number(int(47 * g_Widget->widthScalabilityFactor())) + "px;"
                                                                                                                           "background: transparent;"
                                                                                                                           "margin: 0px, 0px, 0px, 0px;"
                                                                                                                           "padding: 0px, 0px, 0px, 0px;"
                                                                                                                           "}"
                                                                                                                           "QScrollBar::handle:vertical{"
                                                                                                                           "border-top-right-radius: " + QString::number(int(10 * g_Widget->widthScalabilityFactor())) + "px;"
                                                                                                                                                                                                                         "border-bottom-right-radius: " + QString::number(int(10 * g_Widget->widthScalabilityFactor())) + "px;"
                                                                                                                                                                                                                                                                                                                          "min-height: " + QString::number(int(47 * g_Widget->heightScalabilityFactor())) + "px;"
                                                                                                                                                                                                                                                                                                                                                                                                            "background: rgba(0, 162, 255, 191);"
                                                                                                                                                                                                                                                                                                                                                                                                            "}"
                                                                                                                                                                                                                                                                                                                                                                                                            "QScrollBar::add-line:vertical{"
                                                                                                                                                                                                                                                                                                                                                                                                            "height: 0px;"
                                                                                                                                                                                                                                                                                                                                                                                                            "width: 0px;"
                                                                                                                                                                                                                                                                                                                                                                                                            "}"
                                                                                                                                                                                                                                                                                                                                                                                                            "QScrollBar::sub-line:vertical{"
                                                                                                                                                                                                                                                                                                                                                                                                            "height: 0px;"
                                                                                                                                                                                                                                                                                                                                                                                                            "width: 0px;"
                                                                                                                                                                                                                                                                                                                                                                                                            "}"
                                                                                                                                                                                                                                                                                                                                                                                                            "QScrollBar::add-page:vertical{"
                                                                                                                                                                                                                                                                                                                                                                                                            "margin-left: " + QString::number(int(18 * g_Widget->widthScalabilityFactor())) + "px;"
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              "margin-right: " + QString::number(int(18 * g_Widget->widthScalabilityFactor())) + "px;"
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 "background: rgba(42, 42, 42, 191);"
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 "}"
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 "QScrollBar::sub-page:vertical{"
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 "margin-left: " + QString::number(int(18 * g_Widget->widthScalabilityFactor())) + "px;"
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   "margin-right: " + QString::number(int(18 * g_Widget->widthScalabilityFactor())) + "px;"
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      "background: rgba(42, 42, 42, 191);"
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      "}"
                                               );
        m_MissedDelegate = new BluetoothRecordDelegate(q);
        m_MissedStandardItemModel = new QStandardItemModel(q);
        m_MissedCustomListView->setVerticalScrollBar(m_MissedCustomScrollBar);
        m_MissedCustomListView->setItemDelegate(m_MissedDelegate);
        m_MissedCustomListView->setModel(m_MissedStandardItemModel);
        g_Widget->geometryFit(175, 54, 596, 328, m_MissedCustomListView);
        m_MissedCustomListView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        QStandardItem* root = m_MissedStandardItemModel->invisibleRootItem();
        const QList<ContactInfo>& list = g_Bluetooth->getRecordList(BRT_Missed);
        for (int i = 0; i < list.size(); ++i) {
            QStandardItem* item = new QStandardItem();
            item->setSizeHint(QSize((596)  * g_Widget->widthScalabilityFactor(), 55 * g_Widget->heightScalabilityFactor()));
            ContactInfo variant;
            variant.name = list.at(i).name;
            variant.phone = list.at(i).phone;
            item->setData(qVariantFromValue(variant), Qt::DisplayRole);
            root->setChild(root->rowCount(), 0, item);
        }
        m_MissedCustomListView->setVisible(true);
        QObject::connect(m_MissedCustomListView, ARKSENDER(listViewItemRelease(const int)),
                         q,                      ARKRECEIVER(onListViewItemRelease(const int)));
    }
}

void BluetoothRecordWidgetPrivate::connectAllSlots()
{
    Q_Q(BluetoothRecordWidget);
    connectSignalAndSlotByNamesake(g_Widget, q, ARKRECEIVER(onWidgetTypeChange(const Widget::Type, const Widget::Type, const QString &)));
    connectSignalAndSlotByNamesake(g_Bluetooth, q, ARKRECEIVER(onConnectStatusChange(const BluetoothConnectStatus)));
    connectSignalAndSlotByNamesake(g_Bluetooth, q, ARKRECEIVER(onRecordChange(const BluetoothRecordType, const QList<ContactInfo>&)));
    connectSignalAndSlotByNamesake(g_Bluetooth, q, ARKRECEIVER(onRecordCountChange(const BluetoothRecordType, const unsigned)));
    connectSignalAndSlotByNamesake(g_Bluetooth, q, ARKRECEIVER(onSyncChange(const BluetoothRecordType, const BluetoothSyncStatus)));
}

BluetoothRecordDelegate::BluetoothRecordDelegate(QObject *parent)
    : CustomItemDelegate(parent)
{
    QString path(":/Images/BluetoothRecordListViewRules.png");
    if (QFile::exists(path)) {
        m_Rules.reset(new QPixmap(path));
    }
    path = QString(":/Images/BluetoothListViewAvatar.png");
    if (QFile::exists(path)) {
        m_Icon.reset(new QPixmap(path));
    }
    path = QString(":/Images/BluetoothRecordListViewItemPress.png");
    if (QFile::exists(path)) {
        m_Press.reset(new QPixmap(path));
    }
}

BluetoothRecordDelegate::~BluetoothRecordDelegate()
{
}

void BluetoothRecordDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (!m_Rules.isNull()) {
        painter->drawPixmap(0, option.rect.y() + option.rect.height() - 2, *m_Rules);
    }
    if (!m_Icon.isNull()) {
        int x = 149 * g_Widget->widthScalabilityFactor() - m_Icon->width();
        painter->drawPixmap(x * 0.5f, option.rect.y() + (option.rect.height() - m_Icon->height()) * 0.5f, *m_Icon);
    }
    if (m_PressIndex == index) {
        if (!m_Press.isNull()) {
            painter->drawPixmap(0, option.rect.y(), *m_Press);
        }
    }
    ContactInfo variant = qVariantValue<ContactInfo>(index.data(Qt::DisplayRole));
    painter->setPen(Qt::white);
    QString text1("%1");
    text1 = text1.arg(index.row() + 1, 3, 10, QChar('0')) + QString(". ") + variant.name;
    QRect textRect1 = option.rect.adjusted(119 * g_Widget->widthScalabilityFactor(), 0, 149 * g_Widget->widthScalabilityFactor(), 0);
    QFontMetrics fontMetrics(painter->font());
    text1 = fontMetrics.elidedText(text1, Qt::ElideRight, textRect1.width());
    qDebug() << text1 << variant.name;
    painter->drawText(textRect1, Qt::AlignLeft | Qt::AlignVCenter, text1);
    QString text2 = variant.phone;
    QRect textRect2 = option.rect.adjusted(325 * g_Widget->widthScalabilityFactor(), 0, 298 * g_Widget->widthScalabilityFactor(), 0);
    text2 = fontMetrics.elidedText(text2, Qt::ElideRight, textRect2.width());
    painter->drawText(textRect2, Qt::AlignLeft | Qt::AlignVCenter, text2);
}

void BluetoothRecordDelegate::onPressIndexChanged(const QModelIndex &index)
{
    m_PressIndex = index;
}
