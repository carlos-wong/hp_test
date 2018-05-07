#include "BluetoothService.h"
#include <QCoreApplication>
#include <QMetaObject>
#include <stdio.h>
#include <assert.h>

class BluetoothServicePrivate
{
    Q_DISABLE_COPY(BluetoothServicePrivate)
public:
    explicit BluetoothServicePrivate(BluetoothService* parent);
    ~BluetoothServicePrivate();
    void initialize();
    void onArkProtocol(const ArkProtocolWrapper& protocol);
private:
    BluetoothService* m_Parent;
};

BluetoothService::BluetoothService(QObject *parent)
    : QObject(parent)
    , m_Private(new BluetoothServicePrivate(this))
{
}

BluetoothService::~BluetoothService()
{
}

BluetoothServicePrivate::BluetoothServicePrivate(BluetoothService *parent)
    : m_Parent(parent)
{
    initialize();
}

BluetoothServicePrivate::~BluetoothServicePrivate()
{
}

void BluetoothServicePrivate::initialize()
{
}

void BluetoothServicePrivate::onArkProtocol(const ArkProtocolWrapper &protocol)
{
}

