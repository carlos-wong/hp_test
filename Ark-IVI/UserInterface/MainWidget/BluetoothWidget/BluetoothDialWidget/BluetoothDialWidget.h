#ifndef BLUETOOTHDIALWIDGET_H
#define BLUETOOTHDIALWIDGET_H

#include "BusinessLogic/Bluetooth.h"
#include "BusinessLogic/Widget.h"
#include <QWidget>

class BluetoothDialWidgetPrivate;
class BluetoothDialWidget
        : private QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(BluetoothDialWidget)
public:
    explicit BluetoothDialWidget(QWidget *parent = NULL);
    ~BluetoothDialWidget();
protected:
    void resizeEvent(QResizeEvent* event);
protected slots:
    void onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status);
    void onConnectStatusChange(const BluetoothConnectStatus status);
    void onVoiceChange(const BluetoothVoiceMode mode);
    void onDialInfo(const BluetoothRecordType type, const QString& name, const QString& phone);
private slots:
    void onToolButtonRelease();
    void onToolButtonLongPressed();
private:
    Q_DECLARE_PRIVATE(BluetoothDialWidget)
    BluetoothDialWidgetPrivate* const d_ptr;
};

#endif // BLUETOOTHDIALWIDGET_H
