#ifndef BLUETOOTHWIDGET_H
#define BLUETOOTHWIDGET_H

#include "BusinessLogic/Widget.h"
#include "BusinessLogic/Bluetooth.h"
#include <QWidget>

class BluetoothWidgetPrivate;
class BluetoothWidget
        : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(BluetoothWidget)
public:
    explicit BluetoothWidget(QWidget *parent = NULL);
    ~BluetoothWidget();
protected:
    void resizeEvent(QResizeEvent* event);
protected slots:
    void onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status);
    void onConnectStatusChange(const BluetoothConnectStatus status);
    void onPowerChange(const BluetoothPairedMode mode);
    void onLinkStatusChange(const int type, const int status);
private slots:
    void onToolButtonRelease();
    void onTimeout();
private:
    Q_DECLARE_PRIVATE(BluetoothWidget)
    BluetoothWidgetPrivate* const d_ptr;
};

#endif // BLUETOOTHWIDGET_H
