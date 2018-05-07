#ifndef VIEWPAPERWIDGET_H
#define VIEWPAPERWIDGET_H

#include "BusinessLogic/Widget.h"
#include "CustomItemDelegate.h"
#include <QListView>

extern bool g_isClickedTlink;

class ViewPaperWidgetPrivate;
class ViewPaperWidget
        : private QListView
{
    Q_OBJECT
    Q_DISABLE_COPY(ViewPaperWidget)
    Q_PROPERTY(int horizontalOffset READ horizontalOffset WRITE setHorizontalOffset)
public:
    explicit ViewPaperWidget(QWidget* parent = NULL);
    ~ViewPaperWidget();
protected:
    void resizeEvent(QResizeEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
protected slots:
    void onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status);
private slots:
    void onFinished();
    void onButtonRelease(const unsigned int index);
private:
    void setHorizontalOffset(int value);
    Q_DECLARE_PRIVATE(ViewPaperWidget)
    ViewPaperWidgetPrivate* const d_ptr;
};

class ViewPaperItemDelegate : public CustomItemDelegate
{
    Q_OBJECT
    Q_DISABLE_COPY(ViewPaperItemDelegate)
public:
    explicit ViewPaperItemDelegate(QObject *parent = NULL);
    ~ViewPaperItemDelegate();
signals:
    void buttonRelease(const unsigned int index);
protected:
    void paint(QPainter* painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const;
private:
    void mousePressEvent(QMouseEvent* event,
                         QAbstractItemModel *model,
                         const QStyleOptionViewItem &option,
                         const QModelIndex &index);
    void mouseMoveEvent(QMouseEvent* event,
                        QAbstractItemModel *model,
                        const QStyleOptionViewItem &option,
                        const QModelIndex &index);
    void mouseReleaseEvent(QMouseEvent* event,
                           QAbstractItemModel *model,
                           const QStyleOptionViewItem &option,
                           const QModelIndex &index);
    QPoint m_StartMovePoint;
    bool m_Filter;
};

#endif // VIEWPAPERWIDGET_H
