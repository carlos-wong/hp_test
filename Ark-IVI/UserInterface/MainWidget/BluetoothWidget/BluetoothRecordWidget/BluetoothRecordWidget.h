#ifndef BLUETOOTHRECORDWIDGET_H
#define BLUETOOTHRECORDWIDGET_H

#include "BusinessLogic/Bluetooth.h"
#include "BusinessLogic/Widget.h"
#include <QWidget>

class BluetoothRecordWidgetPrivate;
class BluetoothRecordWidget
        : private QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(BluetoothRecordWidget)
public:
    explicit BluetoothRecordWidget(QWidget *parent = NULL);
    ~BluetoothRecordWidget();
protected:
    void resizeEvent(QResizeEvent* event);
protected slots:
    void onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status);
    void onConnectStatusChange(const BluetoothConnectStatus status);
    void onRecordChange(const BluetoothRecordType type, const QList<ContactInfo>& list);
    void onRecordCountChange(const BluetoothRecordType type, const unsigned count);
    void onSyncChange(const BluetoothRecordType type, const BluetoothSyncStatus status);
private slots:
    void onToolButtonRelease();
    void onListViewItemRelease(const int index);
    void onConfirm();
    void onCancel();
private:
    Q_DECLARE_PRIVATE(BluetoothRecordWidget)
    BluetoothRecordWidgetPrivate* const d_ptr;
};

#endif // BLUETOOTHRECORDWIDGET_H
