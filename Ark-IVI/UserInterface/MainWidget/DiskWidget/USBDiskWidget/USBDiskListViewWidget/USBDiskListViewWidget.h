#ifndef USBDISKLISTVIEWWIDGET_H
#define USBDISKLISTVIEWWIDGET_H

#include "BusinessLogic/Multimedia.h"
#include "BusinessLogic/Widget.h"
#include <QWidget>
#include <QScopedPointer>

class USBDiskListViewWidgetPrivate;
class USBDiskListViewWidget
        : private QWidget
{
    Q_OBJECT
public:
    explicit USBDiskListViewWidget(QWidget *parent = NULL);
    ~USBDiskListViewWidget();
protected:
    void resizeEvent(QResizeEvent* event);
protected slots:
    void onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status);
private:
    friend class USBDiskListViewWidgetPrivate;
    QScopedPointer<USBDiskListViewWidgetPrivate> m_Private;
};

#endif // USBDISKLISTVIEWWIDGET_H
