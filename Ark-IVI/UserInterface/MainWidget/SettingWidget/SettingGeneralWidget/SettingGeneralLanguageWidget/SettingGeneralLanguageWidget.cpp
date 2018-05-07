#include "SettingGeneralLanguageWidget.h"
#include "AutoConnect.h"
#include "BmpWidget.h"
#include "BusinessLogic/Widget.h"
#include "BmpButton.h"
#include "CustomItemDelegate.h"
#include "CustomListView.h"
#include <QPainter>
#include <QModelIndex>
#include <QFile>
#include <QStandardItemModel>
#include <QDebug>

namespace SourceString {
static const QString English(QObject::tr("English"));
static const QString Chinese(QObject::tr("Chinese"));
}

class LanguageVariant
{
public:
    LanguageVariant() {}
    ~LanguageVariant() {}
    QString m_Text;
};
Q_DECLARE_METATYPE(LanguageVariant)

class LanguageDelegate : public CustomItemDelegate
{
    Q_DISABLE_COPY(LanguageDelegate)
public:
    explicit LanguageDelegate(QObject* parent = NULL);
    ~LanguageDelegate();
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

class SettingGeneralLanguageWidgetPrivate
{
public:
    explicit SettingGeneralLanguageWidgetPrivate(SettingGeneralLanguageWidget* parent);
    ~SettingGeneralLanguageWidgetPrivate();
    void initializeParent();
    void initializeBasicWidget();
private:
    Q_DECLARE_PUBLIC(SettingGeneralLanguageWidget)
    SettingGeneralLanguageWidget* const q_ptr;
    QStandardItemModel* m_StandardItemModel;
    LanguageDelegate* m_LanguageDelegate;
    CustomListView* m_CustomListView;
};

SettingGeneralLanguageWidget::SettingGeneralLanguageWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new SettingGeneralLanguageWidgetPrivate(this))
{
}

SettingGeneralLanguageWidget::~SettingGeneralLanguageWidget()
{
    if (NULL != d_ptr) {
        delete d_ptr;
    }
}
void SettingGeneralLanguageWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(180, 0, 600, 334, this);
}

void SettingGeneralLanguageWidget::onLanguageTranslateChange(const LanguageType language)
{
    qDebug() << __PRETTY_FUNCTION__ << __LINE__ << language;
    Q_D(SettingGeneralLanguageWidget);
    switch (language) {
    case LT_English: {
        d->m_CustomListView->setCurrentIndex(d->m_CustomListView->model()->index(0, 0, QModelIndex()));
        break;
    }
    case LT_Chinese: {
        d->m_CustomListView->setCurrentIndex(d->m_CustomListView->model()->index(1, 0, QModelIndex()));
        break;
    }
    case LT_Japaness: {
        d->m_CustomListView->setCurrentIndex(d->m_CustomListView->model()->index(2, 0, QModelIndex()));
        break;
    }
    case LT_Korean: {
        d->m_CustomListView->setCurrentIndex(d->m_CustomListView->model()->index(3, 0, QModelIndex()));
        break;
    }
    case LT_Russian: {
        d->m_CustomListView->setCurrentIndex(d->m_CustomListView->model()->index(4, 0, QModelIndex()));
        break;
    }
    default: {
        break;
    }
    }
}

void SettingGeneralLanguageWidget::onListViewItemRelease(const int index)
{
    switch (index) {
    case 0: {
        g_Setting->setLanguage(LT_English);
        break;
    }
    case 1: {
        g_Setting->setLanguage(LT_Chinese);
        break;
    }
    case 2: {
        g_Setting->setLanguage(LT_Japaness);
        break;
    }
    case 3: {
        g_Setting->setLanguage(LT_Korean);
        break;
    }
    case 4: {
        g_Setting->setLanguage(LT_Russian);
        break;
    }
    default: {
        break;
    }
    }
}

SettingGeneralLanguageWidgetPrivate::SettingGeneralLanguageWidgetPrivate(SettingGeneralLanguageWidget *parent)
    : q_ptr(parent)
{
    m_StandardItemModel = NULL;
    m_LanguageDelegate = NULL;
    m_CustomListView = NULL;
    initializeParent();
    initializeBasicWidget();
    Q_Q(SettingGeneralLanguageWidget);
    connectSignalAndSlotByNamesake(g_Setting, q, ARKRECEIVER(onLanguageTranslateChange(const int)));
}

SettingGeneralLanguageWidgetPrivate::~SettingGeneralLanguageWidgetPrivate()
{
}

void SettingGeneralLanguageWidgetPrivate::initializeParent()
{
    Q_Q(SettingGeneralLanguageWidget);
    q->setVisible(false);
}

void SettingGeneralLanguageWidgetPrivate::initializeBasicWidget()
{
    Q_Q(SettingGeneralLanguageWidget);

    m_StandardItemModel = new QStandardItemModel(q);
    m_LanguageDelegate = new LanguageDelegate(q);
    m_CustomListView = new CustomListView(q);
    m_CustomListView->setItemDelegate(m_LanguageDelegate);
    QStandardItem* root = m_StandardItemModel->invisibleRootItem();

    QStandardItem* item = NULL;
    LanguageVariant variant;

    item = new QStandardItem();
    item->setSizeHint(QSize(600 * g_Widget->widthScalabilityFactor(), 60 * g_Widget->heightScalabilityFactor()));
    variant.m_Text = SourceString::English;
    item->setData(qVariantFromValue(variant), Qt::DisplayRole);
    root->setChild(root->rowCount(), 0, item);

    item = new QStandardItem();
    item->setSizeHint(QSize(600 * g_Widget->widthScalabilityFactor(), 60 * g_Widget->heightScalabilityFactor()));
    variant.m_Text = SourceString::Chinese;
    item->setData(qVariantFromValue(variant), Qt::DisplayRole);
    root->setChild(root->rowCount(), 0, item);

    //    item = new QStandardItem();
    //    item->setSizeHint(QSize(600 * g_Widget->widthScalabilityFactor(), 60 * g_Widget->heightScalabilityFactor()));
    //    variant.m_Text = SourceString::Japaness;
    //    item->setData(qVariantFromValue(variant), Qt::DisplayRole);
    //    root->setChild(root->rowCount(), 0, item);

    //    item = new QStandardItem();
    //    item->setSizeHint(QSize(600 * g_Widget->widthScalabilityFactor(), 60 * g_Widget->heightScalabilityFactor()));
    //    variant.m_Text = SourceString::Korean;
    //    item->setData(qVariantFromValue(variant), Qt::DisplayRole);
    //    root->setChild(root->rowCount(), 0, item);

    m_CustomListView->setModel(m_StandardItemModel);
    g_Widget->geometryFit(0, 0, 600, 334, m_CustomListView);
    Qt::ConnectionType type = static_cast<Qt::ConnectionType>(Qt::UniqueConnection | Qt::AutoConnection);
    QObject::connect(m_CustomListView, ARKSENDER(listViewItemRelease(const int)),
                     q,                ARKRECEIVER(onListViewItemRelease(const int)),
                     type);
    m_CustomListView->setCurrentIndex(m_StandardItemModel->index(SettingPersistent::getLanguage(), 0, QModelIndex()));
}

LanguageDelegate::LanguageDelegate(QObject *parent)
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

LanguageDelegate::~LanguageDelegate()
{
}

void LanguageDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->setFont(QFont(QString(), 11 * g_Widget->widthScalabilityFactor()));
    if (!m_Rules.isNull()) {
        painter->drawPixmap(0, option.rect.y() + option.rect.height() - 2, *m_Rules);
    }
    LanguageVariant variant = qVariantValue<LanguageVariant>(index.data(Qt::DisplayRole));
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

void LanguageDelegate::onCurrentIndexChange(const QModelIndex &index)
{
    m_CurrentIndex = index;
    qDebug() << __PRETTY_FUNCTION__ << __LINE__ << m_CurrentIndex;
}

void LanguageDelegate::onPressIndexChanged(const QModelIndex &index)
{
    m_PressIndex = index;
}
