#include "MultimediaListView.h"
#include "CustomItemDelegate.h"
#include "CustomScrollBar.h"
#include "BusinessLogic/Widget.h"
#include "UserInterfaceUtility.h"
#include <QStandardItemModel>
#include <QStandardItem>
#include <QMetaType>
#include <QPainter>
#include <QFontMetrics>
#include <QFile>
#include <QPixmap>

class MultimediaVariant
{
public:
    MultimediaVariant();
    ~MultimediaVariant();
    QString m_Text;
};
Q_DECLARE_METATYPE(MultimediaVariant)

class MultimediaDelegate : public CustomItemDelegate
{
    Q_DISABLE_COPY(MultimediaDelegate)
public:
    explicit MultimediaDelegate(QObject* parent = NULL);
    ~MultimediaDelegate();
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

class MultimediaListViewPrivate
{
    Q_DISABLE_COPY(MultimediaListViewPrivate)
public:
    explicit MultimediaListViewPrivate(MultimediaListView* parent);
    ~MultimediaListViewPrivate();
    void initialize();
    QStandardItemModel* m_StandardItemModel;
    MultimediaDelegate* m_MultimediaDelegate;
    CustomScrollBar* m_CustomScrollBar;
private:
    MultimediaListView* m_Parent;
};

MultimediaListView::MultimediaListView(QWidget *parent)
    : CustomListView(parent)
    , m_Private(new MultimediaListViewPrivate(this))
{
}

MultimediaListView::~MultimediaListView()
{
}

void MultimediaListView::clearListView()
{
    m_Private->m_StandardItemModel->clear();
    verticalScrollBar()->setValue(0);
}

void MultimediaListView::appendListView(QString path)
{
    QStandardItem* root = m_Private->m_StandardItemModel->invisibleRootItem();
    QStandardItem* item = new QStandardItem();
    item->setSizeHint(QSize((531)  * g_Widget->widthScalabilityFactor(), 60 * g_Widget->heightScalabilityFactor()));
    MultimediaVariant variant;
    variant.m_Text = path;
    item->setData(qVariantFromValue(variant), Qt::DisplayRole);
    root->setChild(root->rowCount(), 0, item);
}

void MultimediaListView::setCurrentIndex(const QModelIndex &index)
{
    CustomListView::setCurrentIndex(index);
}

MultimediaListViewPrivate::MultimediaListViewPrivate(MultimediaListView *parent)
    : m_Parent(parent)
{
    m_StandardItemModel = NULL;
    m_MultimediaDelegate = NULL;
    m_CustomScrollBar = NULL;
    initialize();
}

MultimediaListViewPrivate::~MultimediaListViewPrivate()
{
}

void MultimediaListViewPrivate::initialize()
{
    m_CustomScrollBar = new CustomScrollBar(m_Parent);
    m_CustomScrollBar->setStyleSheet("QScrollBar:vertical{"
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
    m_StandardItemModel = new QStandardItemModel(m_Parent);
    m_MultimediaDelegate = new MultimediaDelegate(m_Parent);
    m_Parent->setVerticalScrollBar(m_CustomScrollBar);
    m_Parent->setItemDelegate(m_MultimediaDelegate);
    m_Parent->setModel(m_StandardItemModel);
    m_Parent->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
}

MultimediaVariant::MultimediaVariant()
{
    m_Text.clear();
}

MultimediaVariant::~MultimediaVariant()
{
}

MultimediaDelegate::MultimediaDelegate(QObject *parent)
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

MultimediaDelegate::~MultimediaDelegate()
{
}

void MultimediaDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
//    if (!m_Rules.isNull()) {
//        painter->drawPixmap(0, option.rect.y() + option.rect.height() - 2, *m_Rules);
//    }
    painter->setPen(Qt::white);
    int lineY = option.rect.y() + option.rect.height() - 2;
    painter->drawLine(0, lineY, option.rect.width(), lineY);
    MultimediaVariant variant = qVariantValue<MultimediaVariant>(index.data(Qt::DisplayRole));
    QString text = QString::number(index.row() + 1) + QString(". ") + variant.m_Text;
    QRect textRect = option.rect.adjusted(75 * g_Widget->widthScalabilityFactor(), 0, -75 * g_Widget->widthScalabilityFactor(), 0);
    QFontMetrics fontMetrics(painter->font());
    text = fontMetrics.elidedText(text, Qt::ElideRight, textRect.width());
    if (m_CurrentIndex == index) {
        painter->setPen(QColor(0, 162, 255, 191));
    } else {
        painter->setPen(Qt::white);
    }
    if (m_PressIndex == index) {
//        if (!m_Press.isNull()) {
//            painter->drawPixmap(0, option.rect.y(), *m_Press);
//        }
        QColor c(8,121,165);
        painter->setPen(c);
        painter->setBrush(c);
        if (index.row() == 0) {
             painter->drawRect(option.rect.x()+1, option.rect.y()+2, option.rect.width()-2, option.rect.height()-5);
        }else {
             painter->drawRect(option.rect.x()+1, option.rect.y(), option.rect.width()-2, option.rect.height()-3);
        }
        painter->setPen(Qt::white);
    }
    painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, text);
}

void MultimediaDelegate::onCurrentIndexChange(const QModelIndex &index)
{
    m_CurrentIndex = index;
}

void MultimediaDelegate::onPressIndexChanged(const QModelIndex &index)
{
    m_PressIndex = index;
}
