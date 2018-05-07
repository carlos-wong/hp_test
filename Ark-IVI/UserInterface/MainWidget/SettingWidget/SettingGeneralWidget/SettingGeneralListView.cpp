#include "SettingGeneralListView.h"
#include "CustomItemDelegate.h"
#include "CustomScrollBar.h"
#include "Widget.h"
#include "UserInterfaceUtility.h"
#include <QStandardItemModel>
#include <QStandardItem>
#include <QMetaType>
#include <QPainter>
#include <QFontMetrics>
#include <QFile>
#include <QPixmap>


class MultimediaDelegate : public CustomItemDelegate
{
    Q_DISABLE_COPY(MultimediaDelegate)
public:
    explicit MultimediaDelegate(QWidget* widget = NULL);
    ~MultimediaDelegate();
protected:
    void paint(QPainter* painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const final;
protected slots:
    void onCurrentIndexChange(const QModelIndex &index) final;
    void onPressIndexChanged(const QModelIndex &index) final;
private:
    QModelIndex m_PressIndex;
    QModelIndex m_CurrentIndex;
};

class SettingGeneralListViewPrivate
{
    Q_DISABLE_COPY(SettingGeneralListViewPrivate)
public:
    explicit SettingGeneralListViewPrivate(SettingGeneralListView* parent);
    ~SettingGeneralListViewPrivate();
    void initialize();
    QStandardItemModel* m_StandardItemModel = NULL;
    MultimediaDelegate* m_MultimediaDelegate = NULL;
    CustomScrollBar* m_CustomScrollBar = NULL;
private:
    SettingGeneralListView* m_Parent = NULL;
};

SettingGeneralListView::SettingGeneralListView(QWidget *parent)
    : CustomListView(parent)
    , m_Private(new SettingGeneralListViewPrivate(this))
{
}

SettingGeneralListView::~SettingGeneralListView()
{
}

SettingGeneralListViewPrivate::SettingGeneralListViewPrivate(SettingGeneralListView *parent)
    : m_Parent(parent)
{
    initialize();
}

SettingGeneralListViewPrivate::~SettingGeneralListViewPrivate()
{
}

void SettingGeneralListViewPrivate::initialize()
{
    m_CustomScrollBar = new CustomScrollBar(m_Parent);
    m_CustomScrollBar->setStyleSheet("QScrollBar:vertical{"
                                     "width: 47px;"
                                     "background: transparent;"
                                     "margin: 0px, 0px, 0px, 0px;"
                                     "padding: 0px, 0px, 0px, 0px;"
                                     "}"
                                     "QScrollBar::handle:vertical{"
                                     "border-top-right-radius: 10px;"
                                     "border-bottom-right-radius: 10px;"
                                     "min-height: 47px;"
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
                                     "margin-left: 18px;"
                                     "margin-right: 18px;"
                                     "background: rgba(42, 42, 42, 191);"
                                     "}"
                                     "QScrollBar::sub-page:vertical{"
                                     "margin-left: 18px;"
                                     "margin-right: 18px;"
                                     "background: rgba(42, 42, 42, 191);"
                                     "}"
                                     );
    m_StandardItemModel = new QStandardItemModel(m_Parent);
    m_MultimediaDelegate = new MultimediaDelegate(m_Parent);
    m_Parent->setVerticalScrollBar(m_CustomScrollBar);
    m_Parent->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    m_Parent->setItemDelegate(m_MultimediaDelegate);
    m_Parent->setModel(m_StandardItemModel);
}

MultimediaDelegate::MultimediaDelegate(QWidget *widget)
    : CustomItemDelegate(widget)
{
}

MultimediaDelegate::~MultimediaDelegate()
{
}

void MultimediaDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->save();
    painter->restore();
}

void MultimediaDelegate::onCurrentIndexChange(const QModelIndex &index)
{
    m_CurrentIndex = index;
}

void MultimediaDelegate::onPressIndexChanged(const QModelIndex &index)
{
    m_PressIndex = index;
}
