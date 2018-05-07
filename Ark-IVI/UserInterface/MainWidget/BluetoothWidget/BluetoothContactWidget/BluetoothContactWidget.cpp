#include "BluetoothContactWidget.h"
#include "AutoConnect.h"
#include "BmpWidget.h"
#include "BmpButton.h"
#include "CustomListView.h"
#include "../BluetoothMessageDialog.h"
#include "CustomItemDelegate.h"
#include "CustomScrollBar.h"
#include <QStandardItemModel>
#include <QStyleOptionViewItem>
#include <QDebug>
#include <QPainter>
#include <QFile>
#include <QTimer>

namespace SourceString {
static const QString Clear_List_Sync_Contacts(QObject::tr("Clear List, Sync Contacts?"));
static const QString Cancel_Sync_Other(QObject::tr("Cancel Sync Other?"));
static const QString Wait_For_Accept(QObject::tr("Wait For Accept, Make Sure Phone Permission..."));
static const QString Downloading(QObject::tr("Downloading"));
static const QString Cancelling(QObject::tr("Cancelling"));
static const QString Confirm(QObject::tr("Confirm"));
static const QString Cancel(QObject::tr("Cancel"));
}

class BluetoothContactDelegate : public CustomItemDelegate
{
    Q_DISABLE_COPY(BluetoothContactDelegate)
public:
    explicit BluetoothContactDelegate(QObject* parent = NULL);
    ~BluetoothContactDelegate();
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

class BluetoothContactWidgetPrivate
{
public:
    explicit BluetoothContactWidgetPrivate(BluetoothContactWidget* parent);
    ~BluetoothContactWidgetPrivate();
    void initializeParent();
    void initializeBasicWidget();
    void initializeMessageDialog();
    void connectAllSlots();
private:
    Q_DECLARE_PUBLIC(BluetoothContactWidget)
    BluetoothContactWidget* const q_ptr;
    BmpWidget* m_Background;
    BluetoothMessageDialog* m_MessageDialog;
    BmpButton* m_SyncBtn;
    CustomListView* m_CustomListView;
    QStandardItemModel* m_StandardItemModel;
    BluetoothContactDelegate* m_ContactDelegate;
    CustomScrollBar* m_CustomScrollBar;
};

BluetoothContactWidget::BluetoothContactWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new BluetoothContactWidgetPrivate(this))
{
}

BluetoothContactWidget::~BluetoothContactWidget()
{
    if (NULL != d_ptr) {
        delete d_ptr;
    }
}

void BluetoothContactWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(0, 0, 800, 480, this);
}

void BluetoothContactWidget::onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status)
{
    switch (destinationType) {
    case Widget::T_BluetoothRecord: {
        if (WidgetStatus::Show == status) {
            Q_D(BluetoothContactWidget);
            if (NULL != d->m_MessageDialog) {
                if ((QObject::tr(SourceString::Clear_List_Sync_Contacts.toLocal8Bit().constData()) == d->m_MessageDialog->getTitle())
                        || (QObject::tr(SourceString::Cancel_Sync_Other.toLocal8Bit().constData()) == d->m_MessageDialog->getTitle())) {
                    onCancel();
                }
            }
            setVisible(false);
        }
        break;
    }
    case Widget::T_BluetoothDial:
    case Widget::T_BluetoothMusic:
    case Widget::T_BluetoothSetting: {
        if (WidgetStatus::Show == status) {
            setVisible(false);
        }
        break;
    }
    case Widget::T_BluetoothContact: {
        if (WidgetStatus::RequestShow == status) {
            g_Widget->setWidgetType(Widget::T_BluetoothContact, requestType, WidgetStatus::Show);
        } else if (WidgetStatus::Show == status) {
            lower();
            setVisible(true);
        }
        break;
    }
    default: {
        break;
    }
    }
}

void BluetoothContactWidget::onConnectStatusChange(const BluetoothConnectStatus status)
{
    qDebug() << __PRETTY_FUNCTION__ << status;
    switch (status) {
    case BCS_Idle:
    case BCS_Connecting: {
        Q_D(BluetoothContactWidget);
        if (NULL != d->m_SyncBtn) {
            d->m_SyncBtn->setDisabled(false);
        }
        if (NULL != d->m_StandardItemModel) {
            d->m_StandardItemModel->clear();
        }
        if (NULL != d->m_CustomScrollBar) {
            d->m_CustomScrollBar->setValue(0);
        }
        if (NULL != d->m_MessageDialog) {
            d->m_MessageDialog->setConfirmVisible(true);
            d->m_MessageDialog->setVisible(false);
        }
        setVisible(false);
        break;
    }
    default: {
        break;
    }
    }
}

void BluetoothContactWidget::onRecordChange(const BluetoothRecordType type, const QList<ContactInfo> &list)
{
    Q_D(BluetoothContactWidget);
    if (BRT_PhoneBook == type) {
        d->m_StandardItemModel->clear();
        d->m_CustomScrollBar->setValue(0);
        QStandardItem* root = d->m_StandardItemModel->invisibleRootItem();
        for (int i = 0; i < list.size(); ++i) {
            QStandardItem* item = new QStandardItem();
            item->setSizeHint(QSize((696)  * g_Widget->widthScalabilityFactor(), 55 * g_Widget->heightScalabilityFactor()));
            ContactInfo variant;
            variant.name = list.at(i).name;
            variant.phone = list.at(i).phone;
            item->setData(qVariantFromValue(variant), Qt::DisplayRole);
            root->setChild(root->rowCount(), 0, item);
        }
        if (NULL != d->m_MessageDialog) {
            d->m_MessageDialog->setVisible(false);
            d->m_MessageDialog->setTitle(QObject::tr(SourceString::Clear_List_Sync_Contacts.toLocal8Bit().constData()));
            d->m_MessageDialog->setConfirmVisible(true);
        }
        d->m_SyncBtn->setDisabled(false);
    } else {
        if (NULL != d->m_MessageDialog) {
            d->m_MessageDialog->setVisible(false);
            d->m_SyncBtn->setDisabled(false);
            d->m_MessageDialog->setConfirmVisible(true);
            d->m_MessageDialog->setTitle(QObject::tr(SourceString::Clear_List_Sync_Contacts.toLocal8Bit().constData()));
        }
    }
}

void BluetoothContactWidget::onRecordCountChange(const BluetoothRecordType type, const unsigned count)
{
    if (BRT_PhoneBook == type) {
        Q_D(BluetoothContactWidget);
        d->m_MessageDialog->setTitle(QObject::tr(SourceString::Downloading.toLocal8Bit().constData()) + QString("(") + QString::number(count) + QString(")"));
    }
}

void BluetoothContactWidget::onSyncChange(const BluetoothRecordType type, const BluetoothSyncStatus status)
{
    Q_D(BluetoothContactWidget);
    switch (status) {
    case BSS_Undefine: {
        d->initializeMessageDialog();
        d->m_MessageDialog->setVisible(false);
        d->m_MessageDialog->setConfirmVisible(true);
        d->m_MessageDialog->setTitle(QObject::tr(SourceString::Clear_List_Sync_Contacts.toLocal8Bit().constData()));
        if (NULL != d->m_SyncBtn) {
            d->m_SyncBtn->setDisabled(false);
        }
        break;
    }
    case BSS_Cacelling: {
        if (NULL != d->m_SyncBtn) {
            d->m_SyncBtn->setDisabled(true);
        }
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

void BluetoothContactWidget::onVideoListViewItemRelease(const int index)
{
    Q_D(BluetoothContactWidget);
    if (BCS_Connected == g_Bluetooth->connectStatus()) {
        ContactInfo variant = qVariantValue<ContactInfo>(d->m_StandardItemModel->index(index, 0, QModelIndex()).data(Qt::DisplayRole));
        g_Bluetooth->dialPhone(variant.phone);
    }
}

void BluetoothContactWidget::onToolButtonRelease()
{
    Q_D(BluetoothContactWidget);
    d->m_SyncBtn->setStatus(BmpButton::B_Check);
    d->initializeMessageDialog();
    switch (g_Bluetooth->getSynchronizeType()) {
    case BRT_Outgoing:
    case BRT_Incoming:
    case BRT_Missed: {
        d->m_MessageDialog->setConfirmVisible(true);
        d->m_MessageDialog->setTitle(QObject::tr(SourceString::Cancel_Sync_Other.toLocal8Bit().constData()));
        break;
    }
    default: {
        d->m_MessageDialog->setConfirmVisible(true);
        d->m_MessageDialog->setTitle(QObject::tr(SourceString::Clear_List_Sync_Contacts.toLocal8Bit().constData()));
        break;
    }
    }
    d->m_SyncBtn->setDisabled(true);
    d->m_MessageDialog->setVisible(true);
}

void BluetoothContactWidget::onConfirm()
{
    Q_D(BluetoothContactWidget);
    if (NULL != d->m_MessageDialog) {
        if (QObject::tr(SourceString::Cancel_Sync_Other.toLocal8Bit().constData()) == d->m_MessageDialog->getTitle()) {
            g_Bluetooth->cancelSynchronize(g_Bluetooth->getSynchronizeType());
            d->m_SyncBtn->setDisabled(false);
        } else if (QObject::tr(SourceString::Clear_List_Sync_Contacts.toLocal8Bit().constData()) == d->m_MessageDialog->getTitle()) {
            d->m_StandardItemModel->clear();
            d->m_CustomScrollBar->setValue(0);
            d->m_MessageDialog->setConfirmVisible(false);
            d->m_MessageDialog->setTitle(QObject::tr(SourceString::Wait_For_Accept.toLocal8Bit().constData()));
            g_Bluetooth->synchronizePhoneBook();
            d->m_SyncBtn->setDisabled(true);
        }
    }
}

void BluetoothContactWidget::onCancel()
{
    Q_D(BluetoothContactWidget);
    if (NULL != d->m_MessageDialog) {
        if ((BRT_Undefine == g_Bluetooth->getSynchronizeType())
                || (QObject::tr(SourceString::Cancel_Sync_Other.toLocal8Bit().constData()) == d->m_MessageDialog->getTitle())
                || (QObject::tr(SourceString::Clear_List_Sync_Contacts.toLocal8Bit().constData()) == d->m_MessageDialog->getTitle())) {
            d->m_MessageDialog->setVisible(false);
            d->m_SyncBtn->setDisabled(false);
            d->m_MessageDialog->setConfirmVisible(true);
            d->m_MessageDialog->setTitle(QObject::tr(SourceString::Clear_List_Sync_Contacts.toLocal8Bit().constData()));
        } else {
            d->m_SyncBtn->setDisabled(true);
            g_Bluetooth->cancelSynchronize(g_Bluetooth->getSynchronizeType());
        }
    }
}

BluetoothContactWidgetPrivate::BluetoothContactWidgetPrivate(BluetoothContactWidget *parent)
    : q_ptr(parent)
{
    m_Background = NULL;
    m_MessageDialog = NULL;
    m_SyncBtn = NULL;
    m_CustomListView = NULL;
    m_StandardItemModel = NULL;
    m_ContactDelegate = NULL;
    m_CustomScrollBar = NULL;
    initializeParent();
    initializeBasicWidget();
    connectAllSlots();
}

BluetoothContactWidgetPrivate::~BluetoothContactWidgetPrivate()
{
}

void BluetoothContactWidgetPrivate::initializeParent()
{
    Q_Q(BluetoothContactWidget);
    q->setVisible(false);
}

void BluetoothContactWidgetPrivate::initializeBasicWidget()
{
    Q_Q(BluetoothContactWidget);
    m_Background = new BmpWidget(q);
    m_Background->setBackgroundBmpPath(QString(":/Images/SettingWidgetBackground.png"));
    g_Widget->geometryFit(175, 54, 600, 334, m_Background);
    m_Background->setVisible(true);

    m_SyncBtn = new BmpButton(q);
    m_SyncBtn->setNormalBmpPath(QString(":/Images/BluetoothContactSyncNormal.png"));
    m_SyncBtn->setPressBmpPath(QString(":/Images/BluetoothContactSyncPress.png"));
    m_SyncBtn->setCheckBmpPath(QString(":/Images/BluetoothContactSyncPress.png"));
    g_Widget->geometryFit(29, 54, 140, 70, m_SyncBtn);
    m_SyncBtn->setVisible(true);
    m_SyncBtn->setStatus(BmpButton::B_Check);
    QObject::connect(m_SyncBtn, ARKSENDER(released()),
                     q,         ARKRECEIVER(onToolButtonRelease()));

    m_CustomListView = new CustomListView(q);
    m_CustomScrollBar = new CustomScrollBar(q);
    m_CustomListView->setStyleSheet("QScrollBar:vertical{"
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
    m_ContactDelegate = new BluetoothContactDelegate(q);
    m_StandardItemModel = new QStandardItemModel(q);
    m_CustomListView->setVerticalScrollBar(m_CustomScrollBar);
    m_CustomListView->setItemDelegate(m_ContactDelegate);
    m_CustomListView->setModel(m_StandardItemModel);
    g_Widget->geometryFit(175, 54, 596, 328, m_CustomListView);
    m_CustomListView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    Qt::ConnectionType type = static_cast<Qt::ConnectionType>(Qt::UniqueConnection | Qt::AutoConnection);
    QObject::connect(m_CustomListView, ARKSENDER(listViewItemRelease(const int)),
                     q,                ARKRECEIVER(onVideoListViewItemRelease(const int)),
                     type);
    const QList<ContactInfo>& list = g_Bluetooth->getRecordList(BRT_PhoneBook);
    QStandardItem* root = m_StandardItemModel->invisibleRootItem();
    for (int i = 0; i < list.size(); ++i) {
        QStandardItem* item = new QStandardItem();
        item->setSizeHint(QSize((696)  * g_Widget->widthScalabilityFactor(), 55 * g_Widget->heightScalabilityFactor()));
        ContactInfo variant;
        variant.name = list.at(i).name;
        variant.phone = list.at(i).phone;
        item->setData(qVariantFromValue(variant), Qt::DisplayRole);
        root->setChild(root->rowCount(), 0, item);
    }
    m_CustomListView->setVisible(true);
}

void BluetoothContactWidgetPrivate::initializeMessageDialog()
{
    if (NULL == m_MessageDialog) {
        Q_Q(BluetoothContactWidget);
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

void BluetoothContactWidgetPrivate::connectAllSlots()
{
    Q_Q(BluetoothContactWidget);
    connectSignalAndSlotByNamesake(g_Widget, q, ARKRECEIVER(onWidgetTypeChange(const Widget::Type, const Widget::Type, const QString &)));
    connectSignalAndSlotByNamesake(g_Bluetooth, q, ARKRECEIVER(onConnectStatusChange(const BluetoothConnectStatus)));
    connectSignalAndSlotByNamesake(g_Bluetooth, q, ARKRECEIVER(onRecordChange(const BluetoothRecordType, const QList<ContactInfo>&)));
    connectSignalAndSlotByNamesake(g_Bluetooth, q, ARKRECEIVER(onRecordCountChange(const BluetoothRecordType, const unsigned)));
    connectSignalAndSlotByNamesake(g_Bluetooth, q, ARKRECEIVER(onSyncChange(const BluetoothRecordType, const BluetoothSyncStatus)));
}

BluetoothContactDelegate::BluetoothContactDelegate(QObject *parent)
    : CustomItemDelegate(parent)
{
    QString path(":/Images/BluetoothContactListViewRules.png");
    if (QFile::exists(path)) {
        m_Rules.reset(new QPixmap(path));
    }
    path = QString(":/Images/BluetoothListViewAvatar.png");
    if (QFile::exists(path)) {
        m_Icon.reset(new QPixmap(path));
    }
    path = QString(":/Images/BluetoothContactListViewItemPress.png");
    if (QFile::exists(path)) {
        m_Press.reset(new QPixmap(path));
    }
}

BluetoothContactDelegate::~BluetoothContactDelegate()
{
}

void BluetoothContactDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
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
    painter->drawText(textRect1, Qt::AlignLeft | Qt::AlignVCenter, text1);
    QString text2 = variant.phone;
    QRect textRect2 = option.rect.adjusted(325 * g_Widget->widthScalabilityFactor(), 0, 298 * g_Widget->widthScalabilityFactor(), 0);
    text2 = fontMetrics.elidedText(text2, Qt::ElideRight, textRect2.width());
    painter->drawText(textRect2, Qt::AlignLeft | Qt::AlignVCenter, text2);
}

void BluetoothContactDelegate::onPressIndexChanged(const QModelIndex &index)
{
    m_PressIndex = index;
}
