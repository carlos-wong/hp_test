#ifndef BLUETOOTHSETTINGNUMBERPADWIDGET_H
#define BLUETOOTHSETTINGNUMBERPADWIDGET_H

#include <QWidget>

class BluetoothSettingNumberPadWidgetPrivate;
class BluetoothSettingNumberPadWidget
        : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(BluetoothSettingNumberPadWidget)
public:
    explicit BluetoothSettingNumberPadWidget(QWidget* parent = NULL);
    ~BluetoothSettingNumberPadWidget();
    void setPincode(const QString& pincode);
signals:
    void numberPadKeyType(const QString& type);
protected:
    void paintEvent(QPaintEvent* event);
private slots:
    void onReleased();
private:
    Q_DECLARE_PRIVATE(BluetoothSettingNumberPadWidget)
    BluetoothSettingNumberPadWidgetPrivate* const d_ptr;
};

#endif // BLUETOOTHSETTINGNUMBERPADWIDGET_H
