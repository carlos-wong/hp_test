#ifndef BLUETOOTHPERSISTENT_H
#define BLUETOOTHPERSISTENT_H

#include <QtGlobal>
#include <QString>
#include <QMetaType>

struct ContactInfo {
    QString name;
    QString phone;
};
Q_DECLARE_METATYPE(ContactInfo)

enum BluetoothPlayerStatus {
    BPS_Undefine = -1,
    BPS_Play,
    BPS_Pause,
};

enum BluetoothVoiceMode {
    BVM_Undefine = -1,
    BVM_VoicePhone,
    BVM_VoiceBluetooth,
};

enum BluetoothPairedMode {
    BPM_Undefine = -1,
    BPM_PairedMode,
    BPM_CancelPairedMode,
};

enum BluetoothAutoConnect {
    BAC_Undefine = -1,
    BAC_AutoConnect,
    BAC_CancelAutoConnect,
};

enum BluetoothAutoAnswer {
    BAA_Undefine = -1,
    BAA_AutoAnswer,
    BAA_CancelAutoAnswer,
};

enum BluetoothRecordType {
    BRT_Undefine = -1,
    BRT_PhoneBook,
    BRT_Outgoing,
    BRT_Incoming,
    BRT_Missed,
};

enum BluetoothConnectStatus {
    BCS_Undefine = -1,
    BCS_Idle,
    BCS_Connecting,
    BCS_Connected,
    BCS_Outgoing,
    BCS_Incoming,
    BCS_Talking,
};

enum BluetoothSyncStatus {
    BSS_Undefine = -1,
    BSS_Synchronizing,
    BSS_Cacelling,
};

class BluetoothPersistent
{
    Q_DISABLE_COPY(BluetoothPersistent)
public:
    static void reset();
    void setRemoteAddress(const QString& address);
    QString getRemoteAddress(const bool reload = true);
    explicit BluetoothPersistent();
    ~BluetoothPersistent();
};

#endif // BLUETOOTHPERSISTENT_H
