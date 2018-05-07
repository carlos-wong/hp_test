#include "CustomListView.h"
#include "AutoConnect.h"
#include "UserInterfaceUtility.h"
#include <QEvent>
#include <QMouseEvent>
#include <QScrollBar>
#include <QPoint>
#include <QTimer>

class CustomListViewPrivate
{
public:
    explicit CustomListViewPrivate(CustomListView* parent);
    ~CustomListViewPrivate();
    void initializeTimer();
    QPoint m_StartMovePoint;
    unsigned int m_Threshold;
    bool m_Filter;
    bool m_LongPressFilter;
    QTimer* m_Timer;
private:
    CustomListView* m_Parent;
};

CustomListView::CustomListView(QWidget *parent, const bool tansparent)
    : QListView(parent)
    , m_Private(new CustomListViewPrivate(this))
{
    if (tansparent) {
        QPalette palette = this->palette();
        palette.setBrush(QPalette::Base, QBrush(Qt::NoBrush));
        setPalette(palette);
    }
    setFrameShape(QListView::NoFrame);
    setVerticalScrollMode(QListView::ScrollPerPixel);
    setEditTriggers(QListView::NoEditTriggers);
    setSpacing(0);
    setDragEnabled(false);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setSelectionBehavior(QListView::SelectRows);
    setSelectionMode(QListView::SingleSelection);
}

CustomListView::~CustomListView()
{
}

void CustomListView::setCurrentIndex(const QModelIndex &index)
{
    QListView::setCurrentIndex(index);
    onCurrentIndexChange(index);
    if (isVisible()) {
        update();
    }
}

void CustomListView::setItemDelegate(QAbstractItemDelegate *delegate)
{
    connectSignalAndSlotByNamesake(this, delegate, SLOT(onPressIndexChanged(const QModelIndex &)));
    connectSignalAndSlotByNamesake(this, delegate, SLOT(onCurrentIndexChange(const QModelIndex &)));
    QListView::setItemDelegate(delegate);
}

void CustomListView::enableLongPress(const bool flag)
{
    if (flag) {
        m_Private->initializeTimer();
    }
}

void CustomListView::mousePressEvent(QMouseEvent *event)
{
    m_Private->m_StartMovePoint = event->pos();
    m_Private->m_Filter = false;
    m_Private->m_LongPressFilter = false;
    QModelIndex modelIndex = indexAt(event->pos());
    if (modelIndex.isValid()) {
        emit onPressIndexChanged(modelIndex);
        update(modelIndex);
        if (NULL != m_Private->m_Timer) {
            m_Private->m_Timer->start();
        }
    }
    QListView::mousePressEvent(event);
}

void CustomListView::mouseMoveEvent(QMouseEvent *event)
{
    if (!m_Private->m_LongPressFilter) {
        QPoint relativePos = m_Private->m_StartMovePoint - event->pos();
        int deltaEnd = 0;
        if (QListView::LeftToRight == flow()) {
            deltaEnd = event->pos().x() - m_Private->m_StartMovePoint.x();
        } else {
            deltaEnd = event->pos().y() - m_Private->m_StartMovePoint.y();
        }
        if (m_Private->m_Filter) {
            m_Private->m_StartMovePoint = event->pos();
            if (QListView::LeftToRight == flow()) {
                horizontalScrollBar()->setValue(horizontalOffset() + relativePos.x());
            } else {
                verticalScrollBar()->setValue(verticalOffset() + relativePos.y());
            }
        } else if ((qAbs(deltaEnd) > m_Private->m_Threshold)) {
            if (NULL != m_Private->m_Timer) {
                m_Private->m_Timer->stop();
            }
            emit onPressIndexChanged(QModelIndex());
            if (isVisible()) {
                update();
            }
            m_Private->m_Filter = true;
            m_Private->m_StartMovePoint = event->pos();
            if (QListView::LeftToRight == flow()) {
                if (relativePos.x() > 0) {
                    horizontalScrollBar()->setValue(horizontalOffset() + relativePos.x() - m_Private->m_Threshold);
                } else {
                    horizontalScrollBar()->setValue(horizontalOffset() + relativePos.x() + m_Private->m_Threshold);
                }
            } else {
                if (relativePos.y() > 0) {
                    verticalScrollBar()->setValue(verticalOffset() + relativePos.y() - m_Private->m_Threshold);
                } else {
                    verticalScrollBar()->setValue(verticalOffset() + relativePos.y() + m_Private->m_Threshold);
                }
            }
        }
    }
    QListView::mouseReleaseEvent(event);
}

void CustomListView::mouseReleaseEvent(QMouseEvent *event)
{
    if (!m_Private->m_LongPressFilter) {
        if (NULL != m_Private->m_Timer) {
            m_Private->m_Timer->stop();
        }
        if (!m_Private->m_Filter) {
            QModelIndex modelIndex = indexAt(event->pos());
            if (modelIndex.isValid()) {
                emit listViewItemRelease(modelIndex.row());
            }
        }
    }
    emit onPressIndexChanged(QModelIndex());
    if (isVisible()) {
        update();
    }
    m_Private->m_Filter = false;
    emit listViewRelease();
    QListView::mouseReleaseEvent(event);
}

void CustomListView::onTimeout()
{
    m_Private->m_LongPressFilter = true;
    QModelIndex modelIndex = indexAt(m_Private->m_StartMovePoint);
    if (modelIndex.isValid()) {
        emit listViewItemLongPress(modelIndex.row());
    }
}

CustomListViewPrivate::CustomListViewPrivate(CustomListView *parent)
    : m_Parent(parent)
{
    m_StartMovePoint = QPoint(0, 0);
    m_Threshold = 10;
    m_Filter = false;
    m_LongPressFilter = false;
    m_Timer = NULL;
}

CustomListViewPrivate::~CustomListViewPrivate()
{
}

void CustomListViewPrivate::initializeTimer()
{
    if (NULL == m_Timer) {
        m_Timer = new QTimer(m_Parent);
        m_Timer->setSingleShot(true);
        m_Timer->setInterval(1000);
        QObject::connect(m_Timer,  SIGNAL(timeout()),
                         m_Parent, SLOT(onTimeout()));
    }
}
