#ifndef BLUETOOTHSETTINGPAIREDLISTWIDGET_H
#define BLUETOOTHSETTINGPAIREDLISTWIDGET_H

#include "BusinessLogic/Bluetooth.h"
#include <QWidget>
#include <QList>

class BluetoothSettingPairedListWidgetPrivate;
class BluetoothSettingPairedListWidget
        : private QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(BluetoothSettingPairedListWidget)
public:
    explicit BluetoothSettingPairedListWidget(QWidget *parent = NULL);
    ~BluetoothSettingPairedListWidget();
    void setConnectStatus(const BluetoothConnectStatus status);
protected:
    void resizeEvent(QResizeEvent* event);
protected slots:
    void onConnectStatusChange(const BluetoothConnectStatus status);
    void onPairedListChange(const QMap<unsigned short, QString>& map);
private slots:
    void onConnectBtnRelease();
    void onDeleteBtnRelease();
private:
    Q_DECLARE_PRIVATE(BluetoothSettingPairedListWidget)
    BluetoothSettingPairedListWidgetPrivate* const d_ptr;
};

#endif // BLUETOOTHSETTINGPAIREDLISTWIDGET_H
