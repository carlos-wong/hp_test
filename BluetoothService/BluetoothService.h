#ifndef BLUETOOTHSERVICE_H
#define BLUETOOTHSERVICE_H

#include "ArkProtocolWrapper.h"
#include "BluetoothPersistent.h"
#include <QObject>
#include <QScopedPointer>

class BluetoothServicePrivate;
class BluetoothService : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(BluetoothService)
public:
    explicit BluetoothService(QObject* parent = NULL);
    ~BluetoothService();
private slots:
    void onArkProtocol(const ArkProtocolWrapper& protocol);
private:
    friend class BluetoothServicePrivate;
    QScopedPointer<BluetoothServicePrivate> m_Private;
};

#endif // BLUETOOTHSERVICE_H
