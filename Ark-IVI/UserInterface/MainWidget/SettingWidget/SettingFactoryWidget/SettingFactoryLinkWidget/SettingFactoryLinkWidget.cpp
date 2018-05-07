#include "SettingFactoryLinkWidget.h"
#include "AutoConnect.h"
#include "BmpWidget.h"
#include "BusinessLogic/Widget.h"
#include "BmpButton.h"
#include "CustomItemDelegate.h"
#include "CustomListView.h"
#include "../../SettingMessageDialog.h"
#include <QPainter>
#include <QModelIndex>
#include <QFile>
#include <QStandardItemModel>
#include <QDebug>

namespace SourceString {
static const QString Carlife_Carplay(QObject::tr("Carlife + Carplay"));
static const QString Auto_Carplay(QObject::tr("Auto + Carplay"));
static const QString Link_Tip(QObject::tr("Change Link Item Will Reboot!"));
static const QString Confirm(QObject::tr("Confirm"));
static const QString Cancel(QObject::tr("Cancel"));
static const QString Rebooting(QObject::tr("Rebooting..."));
}

class LinkVariant
{
public:
    LinkVariant() {}
    ~LinkVariant() {}
    QString m_Text;
};
Q_DECLARE_METATYPE(LinkVariant)

class LinkDelegate : public CustomItemDelegate
{
    Q_DISABLE_COPY(LinkDelegate)
public:
    explicit LinkDelegate(QObject* parent = NULL);
    ~LinkDelegate();
protected:
    void paint(QPainter* painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const;
protected slots:
    void onCurrentIndexChange(const QModelIndex &index);
    void onPressIndexChanged(const QModelIndex &index);
private:
    QModelIndex m_PressIndex;
    QModelIndex m_CurrentIndex;
    QScopedPointer<QPixmap> m_Rules;
    QScopedPointer<QPixmap> m_CheckPixmap;
    QScopedPointer<QPixmap> m_Press;
};

class SettingFactoryLinkWidgetPrivate
{
public:
    explicit SettingFactoryLinkWidgetPrivate(SettingFactoryLinkWidget* parent);
    ~SettingFactoryLinkWidgetPrivate();
    void initializeParent();
    void initializeBasicWidget();
    void initializeLinkMessageDialog();
private:
    Q_DECLARE_PUBLIC(SettingFactoryLinkWidget)
    SettingFactoryLinkWidget* const q_ptr;
    QStandardItemModel* m_StandardItemModel;
    LinkDelegate* m_LinkDelegate;
    CustomListView* m_CustomListView;
    SettingMessageDialog* m_LinkDialog;
    CarLinkType m_CarLinkType;
};

SettingFactoryLinkWidget::SettingFactoryLinkWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new SettingFactoryLinkWidgetPrivate(this))
{
}

SettingFactoryLinkWidget::~SettingFactoryLinkWidget()
{
    if (NULL != d_ptr) {
        delete d_ptr;
    }
}

void SettingFactoryLinkWidget::hideDialog()
{
    Q_D(SettingFactoryLinkWidget);
    if (NULL != d->m_LinkDialog) {
        d->m_LinkDialog->setVisible(false);
    }
}

void SettingFactoryLinkWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(180, 52, 600, 334, this);
}

void SettingFactoryLinkWidget::onListViewItemRelease(const int index)
{
    Q_D(SettingFactoryLinkWidget);
    switch (index) {
    case 0: {
        d->m_CarLinkType = CLT_CarlifeCarplay;
        break;
    }
    case 1: {
        d->m_CarLinkType = CLT_AutoCarplay;
        break;
    }
    default: {
        return ;
        break;
    }
    }
    d->initializeLinkMessageDialog();
    d->m_LinkDialog->setVisible(true);
}

void SettingFactoryLinkWidget::onConfirm()
{
    Q_D(SettingFactoryLinkWidget);
    d->m_LinkDialog->setTitle(SourceString::Rebooting);
    d->m_LinkDialog->setBtnVisible(false);
    SettingPersistent::setCarLinkType(d->m_CarLinkType);
    system("sync");
    if (0 != system("reboot")) {
        qCritical() << __PRETTY_FUNCTION__ << "!!!!!!!!!!!factory poweroff faile!!!!!!!!!!!";
        d->m_LinkDialog->setVisible(false);
    }
}

void SettingFactoryLinkWidget::onCancel()
{
    Q_D(SettingFactoryLinkWidget);
    d->m_LinkDialog->setVisible(false);
}

SettingFactoryLinkWidgetPrivate::SettingFactoryLinkWidgetPrivate(SettingFactoryLinkWidget *parent)
    : q_ptr(parent)
{
    m_StandardItemModel = NULL;
    m_LinkDelegate = NULL;
    m_CustomListView = NULL;
    m_LinkDialog = NULL;
    m_CarLinkType = CLT_CarlifeCarplay;
    initializeParent();
    initializeBasicWidget();
}

SettingFactoryLinkWidgetPrivate::~SettingFactoryLinkWidgetPrivate()
{
}

void SettingFactoryLinkWidgetPrivate::initializeParent()
{
    Q_Q(SettingFactoryLinkWidget);
    q->setVisible(false);
}

void SettingFactoryLinkWidgetPrivate::initializeBasicWidget()
{
    Q_Q(SettingFactoryLinkWidget);

    m_StandardItemModel = new QStandardItemModel(q);
    m_LinkDelegate = new LinkDelegate(q);
    m_CustomListView = new CustomListView(q);
    m_CustomListView->setItemDelegate(m_LinkDelegate);
    QStandardItem* root = m_StandardItemModel->invisibleRootItem();

    QStandardItem* item = NULL;
    LinkVariant variant;

    item = new QStandardItem();
    item->setSizeHint(QSize(600 * g_Widget->widthScalabilityFactor(), 60 * g_Widget->heightScalabilityFactor()));
    variant.m_Text = SourceString::Carlife_Carplay;
    item->setData(qVariantFromValue(variant), Qt::DisplayRole);
    root->setChild(root->rowCount(), 0, item);

    item = new QStandardItem();
    item->setSizeHint(QSize(600 * g_Widget->widthScalabilityFactor(), 60 * g_Widget->heightScalabilityFactor()));
    variant.m_Text = SourceString::Auto_Carplay;
    item->setData(qVariantFromValue(variant), Qt::DisplayRole);
    root->setChild(root->rowCount(), 0, item);

    m_CustomListView->setModel(m_StandardItemModel);
    g_Widget->geometryFit(0, 0, 600, 334, m_CustomListView);
    Qt::ConnectionType type = static_cast<Qt::ConnectionType>(Qt::UniqueConnection | Qt::AutoConnection);
    QObject::connect(m_CustomListView, ARKSENDER(listViewItemRelease(const int)),
                     q,                ARKRECEIVER(onListViewItemRelease(const int)),
                     type);
    m_CustomListView->setCurrentIndex(m_StandardItemModel->index(SettingPersistent::getCarLinkType(), 0, QModelIndex()));
}

void SettingFactoryLinkWidgetPrivate::initializeLinkMessageDialog()
{
    if (NULL == m_LinkDialog) {
        Q_Q(SettingFactoryLinkWidget);
        m_LinkDialog = new SettingMessageDialog(q);
        m_LinkDialog->setTitle(SourceString::Link_Tip);
        m_LinkDialog->setConfirm(SourceString::Confirm);
        m_LinkDialog->setCancel(SourceString::Cancel);
        m_LinkDialog->setVisible(false);
        QObject::connect(m_LinkDialog, ARKSENDER(confirm()),
                         q,            ARKRECEIVER(onConfirm()));
        QObject::connect(m_LinkDialog, ARKSENDER(cancel()),
                         q,            ARKRECEIVER(onCancel()));
    }
}

LinkDelegate::LinkDelegate(QObject *parent)
    : CustomItemDelegate(parent)
{
    QString path(":/Images/MultimediaListViewRules.png");
    if (QFile::exists(path)) {
        m_Rules.reset(new QPixmap(path));
    }
    path = QString(":/Images/MultimediaListViewItemPress.png");
    if (QFile::exists(path)) {
        m_Press.reset(new QPixmap(path));
    }
}

LinkDelegate::~LinkDelegate()
{
}

void LinkDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->setFont(QFont(QString(), 11 * g_Widget->widthScalabilityFactor()));
    if (!m_Rules.isNull()) {
        painter->drawPixmap(0, option.rect.y() + option.rect.height() - 2, *m_Rules);
    }
    LinkVariant variant = qVariantValue<LinkVariant>(index.data(Qt::DisplayRole));
    if (m_CurrentIndex == index) {
        painter->setPen(QColor(0, 162, 255, 191));
    } else {
        painter->setPen(Qt::white);
    }
    if (m_PressIndex == index) {
        if (!m_Press.isNull()) {
            painter->drawPixmap(0, option.rect.y(), *m_Press);
        }
        painter->setPen(Qt::white);
    }
    painter->drawText(option.rect, Qt::AlignCenter, QObject::tr(variant.m_Text.toLocal8Bit().constData()));
}

void LinkDelegate::onCurrentIndexChange(const QModelIndex &index)
{
    m_CurrentIndex = index;
    qDebug() << __PRETTY_FUNCTION__ << __LINE__ << m_CurrentIndex;
}

void LinkDelegate::onPressIndexChanged(const QModelIndex &index)
{
    m_PressIndex = index;
}
