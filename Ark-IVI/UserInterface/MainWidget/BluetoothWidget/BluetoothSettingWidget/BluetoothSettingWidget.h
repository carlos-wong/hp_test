#ifndef BLUETOOTHSETTINGWIDGET_H
#define BLUETOOTHSETTINGWIDGET_H

#include "BusinessLogic/Widget.h"
#include "BusinessLogic/Bluetooth.h"
#include <QWidget>

class BluetoothSettingWidgetPrivate;
class BluetoothSettingWidget
        : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(BluetoothSettingWidget)
public:
    explicit BluetoothSettingWidget(QWidget *parent = NULL);
    ~BluetoothSettingWidget();
protected:
    void resizeEvent(QResizeEvent* event);
protected slots:
    void onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status);
    void onConnectStatusChange(const BluetoothConnectStatus status);
    void onPinCodeChange(const QString& name);
    void onLocalDeviceNameChange(const QString& name);
    void onRemoteDeviceNameChange(const QString& name);
private slots:
    void onAlphabetPadKeyType(const QString& type);
    void onNumberPadKeyType(const QString& type);
    void onPopAlphabetPad();
    void onPopNumberPad();
private:
    Q_DECLARE_PRIVATE(BluetoothSettingWidget)
    BluetoothSettingWidgetPrivate* const d_ptr;
};

#endif // BLUETOOTHSETTINGWIDGET_H
