#ifndef BLUETOOTHSETTINGINFORMATIONWIDGET_H
#define BLUETOOTHSETTINGINFORMATIONWIDGET_H

#include "BusinessLogic/Bluetooth.h"
#include <QWidget>

class BluetoothSettingInformationWidgetPrivate;
class BluetoothSettingInformationWidget
        : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(BluetoothSettingInformationWidget)
public:
    explicit BluetoothSettingInformationWidget(QWidget *parent = NULL);
    ~BluetoothSettingInformationWidget();
    void setDeviceName(const QString& name);
    void setPinCode(const QString& pin);
signals:
    void popAlphabetPad();
    void popNumberPad();
protected:
    void resizeEvent(QResizeEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
protected slots:
    void onPowerChange(const BluetoothPairedMode mode);
    void onAutoConnectChange(const BluetoothAutoConnect mode);
    void onAutoAnswerChange(const BluetoothAutoAnswer mode);
private slots:
    void onBmpButtonRelease();
private:
    friend class BluetoothSettingWidget;
    Q_DECLARE_PRIVATE(BluetoothSettingInformationWidget)
    BluetoothSettingInformationWidgetPrivate* const d_ptr;
};

#endif // BLUETOOTHSETTINGINFORMATIONWIDGET_H
