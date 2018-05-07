#ifndef USBDISKTOOLWIDGET_H
#define USBDISKTOOLWIDGET_H

#include "BusinessLogic/Widget.h"
#include "BusinessLogic/Multimedia.h"
#include <QWidget>
#include <QScopedPointer>

class USBDiskToolWidgetPrivate;
class USBDiskToolWidget
        : protected QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(USBDiskToolWidget)
public:
    explicit USBDiskToolWidget(QWidget *parent = NULL);
    ~USBDiskToolWidget();
protected:
    void resizeEvent(QResizeEvent* event);
protected slots:
    void onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status);
    void onDeviceWatcherStatus(const int type, const int status);
    //void onDeviceWatcherStatus(const DeviceWatcherType type, const DeviceWatcherStatus status);
private slots:
    void onToolButtonRelease();
private:
    friend class USBDiskToolWidgetPrivate;
    QScopedPointer<USBDiskToolWidgetPrivate> m_Private;
};

#endif // USBDISKTOOLWIDGET_H
