#ifndef BLUETOOTHSETTINGSTATUSWIDGET_H
#define BLUETOOTHSETTINGSTATUSWIDGET_H

#include "BusinessLogic/Bluetooth.h"
#include <QWidget>

class BluetoothSettingStatusWidgetPrivate;
class BluetoothSettingStatusWidget
        : private QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(BluetoothSettingStatusWidget)
public:
    explicit BluetoothSettingStatusWidget(QWidget *parent = NULL);
    ~BluetoothSettingStatusWidget();
    void setConnectStatus(const BluetoothConnectStatus status, const QString& name);
protected:
    void resizeEvent(QResizeEvent* event);
private slots:
    void onToolButtonRelease();
private:
    Q_DECLARE_PRIVATE(BluetoothSettingStatusWidget)
    BluetoothSettingStatusWidgetPrivate* const d_ptr;
};

#endif // BLUETOOTHSETTINGSTATUSWIDGET_H
