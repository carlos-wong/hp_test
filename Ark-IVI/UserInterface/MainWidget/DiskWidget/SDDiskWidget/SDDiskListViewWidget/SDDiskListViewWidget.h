#ifndef SDDISKLISTVIEWWIDGET_H
#define SDDISKLISTVIEWWIDGET_H

#include "BusinessLogic/Multimedia.h"
#include "BusinessLogic/Widget.h"
#include <QWidget>
#include <QScopedPointer>

class SDDiskListViewWidgetPrivate;
class SDDiskListViewWidget
        : private QWidget
{
    Q_OBJECT
public:
    explicit SDDiskListViewWidget(QWidget *parent = NULL);
    ~SDDiskListViewWidget();
protected:
    void resizeEvent(QResizeEvent* event);
protected slots:
    void onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status);
private:
    friend class SDDiskListViewWidgetPrivate;
    QScopedPointer<SDDiskListViewWidgetPrivate> m_Private;
};

#endif // SDDISKLISTVIEWWIDGET_H
