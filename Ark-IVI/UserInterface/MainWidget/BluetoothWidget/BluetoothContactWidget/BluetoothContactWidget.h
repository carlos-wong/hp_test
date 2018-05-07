#ifndef BLUETOOTHCONTACTWIDGET_H
#define BLUETOOTHCONTACTWIDGET_H

#include "BusinessLogic/Bluetooth.h"
#include "BusinessLogic/Widget.h"
#include <QWidget>

class BluetoothContactWidgetPrivate;
class BluetoothContactWidget
        : private QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(BluetoothContactWidget)
public:
    explicit BluetoothContactWidget(QWidget *parent = NULL);
    ~BluetoothContactWidget();
protected:
    void resizeEvent(QResizeEvent* event);
protected slots:
    void onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status);
    void onConnectStatusChange(const BluetoothConnectStatus status);
    void onRecordChange(const BluetoothRecordType type, const QList<ContactInfo>& list);
    void onRecordCountChange(const BluetoothRecordType type, const unsigned count);
    void onSyncChange(const BluetoothRecordType type, const BluetoothSyncStatus status);
private slots:
    void onVideoListViewItemRelease(const int index);
    void onToolButtonRelease();
    void onConfirm();
    void onCancel();
private:
    Q_DECLARE_PRIVATE(BluetoothContactWidget)
    BluetoothContactWidgetPrivate* const d_ptr;
};

#endif // BLUETOOTHCONTACTWIDGET_H
