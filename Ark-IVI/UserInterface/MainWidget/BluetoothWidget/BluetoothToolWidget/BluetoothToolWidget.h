#ifndef BLUETOOTHTOOLWIDGET_H
#define BLUETOOTHTOOLWIDGET_H

#include "BusinessLogic/Widget.h"
#include "BusinessLogic/Bluetooth.h"
#include <QWidget>

class BluetoothToolWidgetPrivate;
class BluetoothToolWidget
        : private QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(BluetoothToolWidget)
public:
    explicit BluetoothToolWidget(QWidget *parent = NULL);
    ~BluetoothToolWidget();
protected:
    void resizeEvent(QResizeEvent* event);
protected slots:
    void onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status);
    void onConnectStatusChange(const BluetoothConnectStatus status);
private slots:
    void onToolButtonRelease();
private:
    Q_DECLARE_PRIVATE(BluetoothToolWidget)
    BluetoothToolWidgetPrivate* const d_ptr;
};

#endif // BLUETOOTHTOOLWIDGET_H
