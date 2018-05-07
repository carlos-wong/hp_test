#include "SettingGeneralTimeColumeWidget.h"
#include "AutoConnect.h"
#include "BmpWidget.h"
#include "BmpButton.h"
#include "CustomItemDelegate.h"
#include "CustomListView.h"
#include "TextWidget.h"
#include <QPainter>
#include <QStandardItemModel>
#include <QStyleOptionViewItem>
#include <QModelIndex>
#include <QScrollBar>
#include <QDebug>

class TimeVariant {
public:
    TimeVariant() {}
    ~TimeVariant() {}
    QString m_Text;
};
Q_DECLARE_METATYPE(TimeVariant)

class TimeDelegate : public CustomItemDelegate
{
    Q_DISABLE_COPY(TimeDelegate)
public:
    explicit TimeDelegate(QObject* parent = NULL);
    ~TimeDelegate();
protected:
    void paint(QPainter* painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const;
protected slots:
    void onCurrentIndexChange(const QModelIndex &index);
private:
    QModelIndex m_CurrentIndex;
};

class SettingGeneralTimeColumeWidgetPrivate
{
public:
    explicit SettingGeneralTimeColumeWidgetPrivate(SettingGeneralTimeColumeWidget* parent);
    ~SettingGeneralTimeColumeWidgetPrivate();
    void initializeParent();
    void initializeBasicWidget();
private:
    Q_DECLARE_PUBLIC(SettingGeneralTimeColumeWidget)
    SettingGeneralTimeColumeWidget* const q_ptr;
    BmpWidget* m_Background;
    QStandardItemModel* m_StandardItemModel;
    TimeDelegate* m_TimeDelegate;
    CustomListView* m_CustomListView;
    TextWidget* m_Text;
};

SettingGeneralTimeColumeWidget::SettingGeneralTimeColumeWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new SettingGeneralTimeColumeWidgetPrivate(this))
{
}

SettingGeneralTimeColumeWidget::~SettingGeneralTimeColumeWidget()
{
    if (NULL != d_ptr) {
        delete d_ptr;
    }
}

void SettingGeneralTimeColumeWidget::setText(const QString &text)
{
    Q_D(SettingGeneralTimeColumeWidget);
    d->m_Text->setText(text);
}

void SettingGeneralTimeColumeWidget::replaceListView(const QList<QString> &list)
{
    Q_D(SettingGeneralTimeColumeWidget);
    d->m_StandardItemModel->clear();
    d->m_CustomListView->verticalScrollBar()->setValue(0);
    QStandardItem* root = d->m_StandardItemModel->invisibleRootItem();
    for (int i = 0; i < 2; ++i) {
        QStandardItem* item = new QStandardItem();
        item->setSizeHint(QSize(82 * g_Widget->widthScalabilityFactor(), 38 * g_Widget->heightScalabilityFactor()));
        TimeVariant variant;
        variant.m_Text = QString();
        item->setData(qVariantFromValue(variant), Qt::DisplayRole);
        root->setChild(root->rowCount(), 0, item);
    }
    for (QList<QString>::const_iterator iter = list.begin(); iter != list.end(); ++iter) {
        QStandardItem* item = new QStandardItem();
        item->setSizeHint(QSize(82 * g_Widget->widthScalabilityFactor(), 38 * g_Widget->heightScalabilityFactor()));
        TimeVariant variant;
        variant.m_Text = *iter;
        item->setData(qVariantFromValue(variant), Qt::DisplayRole);
        root->setChild(root->rowCount(), 0, item);
    }
    for (int i = 0; i < 2; ++i) {
        QStandardItem* item = new QStandardItem();
        item->setSizeHint(QSize(82 * g_Widget->widthScalabilityFactor(), 38 * g_Widget->heightScalabilityFactor()));
        TimeVariant variant;
        variant.m_Text = QString();
        item->setData(qVariantFromValue(variant), Qt::DisplayRole);
        root->setChild(root->rowCount(), 0, item);
    }
}

void SettingGeneralTimeColumeWidget::scrollToIndex(const int index)
{
    Q_D(SettingGeneralTimeColumeWidget);
    if (d->m_CustomListView->currentIndex().row() != index + 2) {
        d->m_CustomListView->setCurrentIndex(d->m_StandardItemModel->index(index + 2, 0, QModelIndex()));
        d->m_CustomListView->scrollTo(d->m_StandardItemModel->index(index + 2, 0, QModelIndex()), CustomListView::PositionAtCenter);
        if (isEnabled()) {
            indexChange(index);
        }
    }
}

int SettingGeneralTimeColumeWidget::currentIndex()
{
    Q_D(SettingGeneralTimeColumeWidget);
    return d->m_CustomListView->currentIndex().row() - 2;
}

void SettingGeneralTimeColumeWidget::setEnabled(const bool flag)
{
    Q_D(SettingGeneralTimeColumeWidget);
    d->m_CustomListView->setEnabled(flag);
    QWidget::setEnabled(flag);
}

void SettingGeneralTimeColumeWidget::onListViewRelease()
{
    Q_D(SettingGeneralTimeColumeWidget);
    int value = 38 * g_Widget->heightScalabilityFactor();
    int remainder = d->m_CustomListView->verticalScrollBar()->value() % value;
    int offset = d->m_CustomListView->verticalScrollBar()->value() - remainder;
    if (remainder > (value / 2)) {
        offset += value;
    }
    scrollToIndex(offset / value);
}

SettingGeneralTimeColumeWidgetPrivate::SettingGeneralTimeColumeWidgetPrivate(SettingGeneralTimeColumeWidget *parent)
    : q_ptr(parent)
{
    m_Background = NULL;
    m_StandardItemModel = NULL;
    m_TimeDelegate = NULL;
    m_CustomListView = NULL;
    m_Text = NULL;
    initializeParent();
    initializeBasicWidget();
}

SettingGeneralTimeColumeWidgetPrivate::~SettingGeneralTimeColumeWidgetPrivate()
{
}

void SettingGeneralTimeColumeWidgetPrivate::initializeParent()
{
    Q_Q(SettingGeneralTimeColumeWidget);
    q->setVisible(false);
    q->resize(QSize(82 * g_Widget->widthScalabilityFactor(), 219 * g_Widget->heightScalabilityFactor()));
}

void SettingGeneralTimeColumeWidgetPrivate::initializeBasicWidget()
{
    Q_Q(SettingGeneralTimeColumeWidget);
    m_Background = new BmpWidget(q);
    m_Background->setBackgroundBmpPath(QString(":/Images/SettingGeneralTimeColumeBackground.png"));
    g_Widget->geometryFit(0, 25, 82, 194, m_Background);
    m_Background->setVisible(true);

    m_Text = new TextWidget(q);
    g_Widget->geometryFit(0, 0, 82, 25, m_Text);
    m_Text->setVisible(true);

    m_StandardItemModel = new QStandardItemModel(q);
    m_CustomListView = new CustomListView(q);
    m_TimeDelegate = new TimeDelegate(q);
    m_CustomListView->setItemDelegate(m_TimeDelegate);
    m_CustomListView->setModel(m_StandardItemModel);
    g_Widget->geometryFit(0, 25, 82, 194, m_CustomListView);
    m_CustomListView->setVisible(true);
    Qt::ConnectionType type = static_cast<Qt::ConnectionType>(Qt::UniqueConnection | Qt::AutoConnection);
    QObject::connect(m_CustomListView, ARKSENDER(listViewRelease()),
                     q,                ARKRECEIVER(onListViewRelease()),
                     type);
}

TimeDelegate::TimeDelegate(QObject *parent)
    : CustomItemDelegate(parent)
{
}

TimeDelegate::~TimeDelegate()
{
}

void TimeDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->setFont(QFont(QString(), 11 * g_Widget->widthScalabilityFactor()));
    TimeVariant variant = qVariantValue<TimeVariant>(index.data(Qt::DisplayRole));
    if (m_CurrentIndex == index) {
        painter->setPen(QColor(0, 162, 255, 191));
    } else {
        painter->setPen(Qt::white);
    }
    painter->drawText(option.rect, Qt::AlignCenter, variant.m_Text);
}

void TimeDelegate::onCurrentIndexChange(const QModelIndex &index)
{
    m_CurrentIndex = index;
}

