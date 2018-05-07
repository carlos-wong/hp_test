#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include "BluetoothService.h"
#include <QObject>
#include <QScopedPointer>
#include <QCoreApplication>
#include <QMap>

class BluetoothPrivate;
class Bluetooth
        : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(Bluetooth)
#ifdef g_Bluetooth
#undef g_Bluetooth
#endif
#define g_Bluetooth (Bluetooth::instance())
public:
    inline static Bluetooth* instance() {
        static Bluetooth* bluetooth(new Bluetooth(qApp));
        return bluetooth;
    }
    void reset();
    bool trusteeshipNativeBluetooth();
    void enableTrusteeshipNativeBluetooth();
    void disableTrusteeshipNativeBluetooth();
    QString pinCode();
    QString localDeviceName();
    QString localDeviceAddress();
    QString remoteDeviceName();
    QString remoteDeviceAddress();
    BluetoothConnectStatus connectStatus();
    const QList<ContactInfo>& getRecordList(const BluetoothRecordType type);
    const QMap<unsigned short, QString>& getPairedList();
    void initialize();
    void initializeMusic();
    void musicToggle();
    void musicPrevious();
    void musicNext();
    void connectRemoteDevice(const unsigned short index);
    void disconnectRemoteDevice();
    void deleteRemoteDevice(const unsigned short index);
    BluetoothRecordType getSynchronizeType();
    bool cancellingSynchronize();
    void cancelSynchronize(BluetoothRecordType type);
    void synchronizePhoneBook();
    void synchronizeOutgoing();
    void synchronizeIncoming();
    void synchronizeMissed();
    void setPincode(const QString& pincode);
    void setLocalDeviceName(const QString& devicename);
    void dialPhone(const QString& phone);
    void dialNumber(const QChar& number);
    BluetoothVoiceMode getVoiceMode();
    void voiceToggleSwitch();
    void redialLastPhone();
    void pickupPhone();
    void hanupPhone();
    BluetoothPairedMode getPowerStatus();
    void powerOn();
    void powerOff();
    BluetoothAutoConnect getAutoConnectStatus();
    void autoConnectOn();
    void autoConnectOff();
    BluetoothAutoAnswer getAutoAnswer();
    void autoAnswerOn();
    void autoAnswerOff();
signals:
    void onPinCodeChange(const QString& name);
    void onLocalDeviceNameChange(const QString& name);
    void onRemoteDeviceNameChange(const QString& name);
    void onConnectStatusChange(const BluetoothConnectStatus status);
    void onMusicStatusChange(const QString& musicName, const BluetoothPlayerStatus status);
    void onRecordChange(const BluetoothRecordType type, const QList<ContactInfo>& list);
    void onRecordCountChange(const BluetoothRecordType type, const unsigned count);
    void onDialInfo(const BluetoothRecordType type, const QString& name, const QString& phone);
    void onVoiceChange(const BluetoothVoiceMode mode);
    void onPowerChange(const BluetoothPairedMode mode);
    void onAutoConnectChange(const BluetoothAutoConnect mode);
    void onAutoAnswerChange(const BluetoothAutoAnswer mode);
    void onPairedListChange(const QMap<unsigned short, QString>& map);
    void onSyncChange(const BluetoothRecordType type, const BluetoothSyncStatus status);
private slots:
    void onArkProtocol(const ArkProtocolWrapper& protocol);
    void onHolderChange(const int oldHolder, const int newHolder);
    void onTimeout();
private:
    friend class Setting;
    friend class SettingPrivate;
    void setInnerBluetooth(const ArkBlueTooth* bluetooth);
    explicit Bluetooth(QObject *parent = NULL);
    ~Bluetooth();
    void initializePrivate();
    BluetoothPrivate* const d_ptr;
    Q_DECLARE_PRIVATE(Bluetooth)
};

#endif // BLUETOOTH_H

