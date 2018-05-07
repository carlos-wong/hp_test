#include "Bluetooth.h"
#include "AutoConnect.h"
#include "RunnableThread.h"
#include "UserInterfaceUtility.h"
#include "Setting.h"
#include "Audio.h"
#include "Radio.h"
#include <QList>
#include <QString>
#include <QChar>
#include <QMap>
#include <unistd.h>
#include <QTime>
#include <QTimer>

class BluetoothPrivate
{
public:
    explicit BluetoothPrivate(Bluetooth* parent);
    ~BluetoothPrivate();
    void connectAllSlots();
    void onArkProtocol(const ArkProtocol& protocol);
    void cancelSync();
    void syncPhoneBook();
    void syncOutgoing();
    void syncIncoming();
    void syncMissed();
    //    void initializeAutoAnswerTimer();
    //    void initializeForcePlayTimer();
    void initializeAutoCancelTimer();
    void initializeCancellingTimer();
private:
    Bluetooth* const q_ptr;
    Q_DECLARE_PUBLIC(Bluetooth)
    QString m_Pincode;
    QString m_LocalDeviceName;
    QString m_LocalAddress;
    QString m_RemoteDeviceName;
    QString m_RemoteAddress;
    BluetoothConnectStatus m_BluetoothConnectStatus;
    bool m_Incoming;
    bool m_Outcoming;
    BluetoothPlayerStatus m_BluetoothPlayerStatus;
    QString m_MusicName;
    QTimer* m_AutoCancelTimer;
    BluetoothVoiceMode m_BluetoothVoiceMode;
    BluetoothPairedMode m_BluetoothPairedMode;
    BluetoothAutoConnect m_BluetoothAutoConnect;
    BluetoothAutoAnswer m_BluetoothAutoAnswer;
    QString m_LastNumber;
    QList<struct ContactInfo> m_PhoneBookList;
    QList<struct ContactInfo> m_OutgoingList;
    QList<struct ContactInfo> m_IncomingList;
    QList<struct ContactInfo> m_MissedList;
    QMap<unsigned short, QString> m_PairedMap;
    BluetoothRecordType m_SyncType;
    bool m_TrusteeshipNativeBluetooth;
    QTimer* m_CancellingTimer;
};

void Bluetooth::reset()
{
}

bool Bluetooth::trusteeshipNativeBluetooth()
{
    Q_D(Bluetooth);
    return d->m_TrusteeshipNativeBluetooth;
}

void Bluetooth::enableTrusteeshipNativeBluetooth()
{
    Q_D(Bluetooth);
    d->m_TrusteeshipNativeBluetooth = true;
    if (NULL != d->m_AutoCancelTimer) {
        if (d->m_AutoCancelTimer->isActive()) {

        }
    }
}

void Bluetooth::disableTrusteeshipNativeBluetooth()
{
    Q_D(Bluetooth);
    d->m_TrusteeshipNativeBluetooth = false;
}

QString Bluetooth::pinCode()
{
    Q_D(Bluetooth);
    qDebug() << __PRETTY_FUNCTION__ << d->m_Pincode;
    return d->m_Pincode;
}

QString Bluetooth::localDeviceName()
{
    Q_D(Bluetooth);
    qDebug() << __PRETTY_FUNCTION__ << d->m_LocalDeviceName;
    return d->m_LocalDeviceName;
}

QString Bluetooth::localDeviceAddress()
{
    Q_D(Bluetooth);
    qDebug() << __PRETTY_FUNCTION__ << d->m_LocalAddress;
    return d->m_LocalAddress;
}

QString Bluetooth::remoteDeviceName()
{
    Q_D(Bluetooth);
    return d->m_RemoteDeviceName;
}

QString Bluetooth::remoteDeviceAddress()
{
    Q_D(Bluetooth);
    return d->m_RemoteAddress;
}

BluetoothConnectStatus Bluetooth::connectStatus()
{
    Q_D(Bluetooth);
    return d->m_BluetoothConnectStatus;
}

const QList<ContactInfo> &Bluetooth::getRecordList(const BluetoothRecordType type)
{
    Q_D(Bluetooth);
    switch (type) {
    case BRT_PhoneBook: {
        return d->m_PhoneBookList;
        break;
    }
    case BRT_Outgoing: {
        return d->m_OutgoingList;
        break;
    }
    case BRT_Incoming: {
        return d->m_IncomingList;
        break;
    }
    case BRT_Missed: {
        return d->m_MissedList;
        break;
    }
    default: {
        break;
    }
    }
}

const QMap<unsigned short, QString> &Bluetooth::getPairedList()
{
    Q_D(Bluetooth);
    return d->m_PairedMap;
}

void Bluetooth::initialize()
{
    do {
        Q_D(Bluetooth);
        ArkBlueTooth bluetooth;
        bluetooth.type = ABT_MusicPlay;
        bluetooth.length = 0;
        bluetooth.data = NULL;
        ArkProtocol protocol;
        protocol.type = AT_Bluetooth;
        protocol.direction = AD_Setter;
        protocol.data = static_cast<void*>(&bluetooth);
        d->onArkProtocol(protocol);
    } while(0);
    do {
        Q_D(Bluetooth);
        struct ArkControl control;
        memset((void*)&control, 0, sizeof(struct ArkControl));
        struct ArkProtocol protocol;
        memset((void*)&protocol, 0, sizeof(struct ArkProtocol));
        protocol.type = AT_Control;
        protocol.direction = AD_Setter;
        protocol.data = &control;
        control.type = ACLT_RequestSourceSwitch;
        unsigned char data[1] = {0x0B};
        control.data = data;
        control.length = 1;
        d->onArkProtocol(protocol);
    } while (0);
    qDebug() << __PRETTY_FUNCTION__ << __LINE__;
    g_Audio->requestAudioSource(AS_BluetoothPhone);
}

void Bluetooth::initializeMusic()
{
    if (AS_BluetoothMusic != g_Audio->getAudioSource()) {
        if (g_Audio->requestAudioSource(AS_BluetoothMusic)) {
            do {
                struct ArkControl control;
                memset((void*)&control, 0, sizeof(struct ArkControl));
                struct ArkProtocol protocol;
                memset((void*)&protocol, 0, sizeof(struct ArkProtocol));
                protocol.type = AT_Control;
                protocol.direction = AD_Setter;
                protocol.data = &control;
                control.type = ACLT_RequestSourceSwitch;
                unsigned char data[1] = {0x0B};
                control.data = data;
                control.length = 1;
                g_Setting->setArkProtocol(ArkProtocolWrapper(protocol));
            } while (0);

            do {
                Q_D(Bluetooth);
                ArkBlueTooth bluetooth;
                bluetooth.type = ABT_MusicPlay;
                bluetooth.length = 0;
                bluetooth.data = NULL;
                ArkProtocol protocol;
                protocol.type = AT_Bluetooth;
                protocol.direction = AD_Setter;
                protocol.data = static_cast<void*>(&bluetooth);
                d->onArkProtocol(protocol);
            } while (0);
        }
    }
}

void Bluetooth::musicToggle()
{
    Q_D(Bluetooth);
    ArkBlueTooth bluetooth;
    if (BPS_Play == d->m_BluetoothPlayerStatus) {
        bluetooth.type = ABT_MusicPause;
    } else {
        bluetooth.type = ABT_MusicPlay;
    }
    bluetooth.length = 0;
    bluetooth.data = NULL;
    ArkProtocol protocol;
    protocol.type = AT_Bluetooth;
    protocol.direction = AD_Setter;
    protocol.data = static_cast<void*>(&bluetooth);
    d->onArkProtocol(protocol);
}

void Bluetooth::musicPrevious()
{
    Q_D(Bluetooth);
    ArkBlueTooth bluetooth;
    bluetooth.type = ABT_MusicPre;
    bluetooth.length = 0;
    bluetooth.data = NULL;
    ArkProtocol protocol;
    protocol.type = AT_Bluetooth;
    protocol.direction = AD_Setter;
    protocol.data = static_cast<void*>(&bluetooth);
    d->onArkProtocol(protocol);
}

void Bluetooth::musicNext()
{
    Q_D(Bluetooth);
    ArkBlueTooth bluetooth;
    bluetooth.type = ABT_MusicNext;
    bluetooth.length = 0;
    bluetooth.data = NULL;
    ArkProtocol protocol;
    protocol.type = AT_Bluetooth;
    protocol.direction = AD_Setter;
    protocol.data = static_cast<void*>(&bluetooth);
    d->onArkProtocol(protocol);
}

void Bluetooth::disconnectRemoteDevice()
{
    Q_D(Bluetooth);
    d->cancelSync();
    ArkBlueTooth bluetooth;
    bluetooth.type = ABT_DisConnect;
    bluetooth.length = 0;
    bluetooth.data = NULL;
    ArkProtocol protocol;
    protocol.type = AT_Bluetooth;
    protocol.direction = AD_Setter;
    protocol.data = static_cast<void*>(&bluetooth);
    d->onArkProtocol(protocol);
}

void Bluetooth::deleteRemoteDevice(const unsigned short index)
{
    Q_D(Bluetooth);
    ArkBlueTooth bluetooth;
    bluetooth.type = ABT_DeletePairedListIndex;
    unsigned char data = index;
    bluetooth.data = &data;
    bluetooth.length = 1;
    ArkProtocol protocol;
    protocol.type = AT_Bluetooth;
    protocol.direction = AD_Setter;
    protocol.data = static_cast<void*>(&bluetooth);
    d->onArkProtocol(protocol);
}

BluetoothRecordType Bluetooth::getSynchronizeType()
{
    Q_D(Bluetooth);
    return d->m_SyncType;
}

bool Bluetooth::cancellingSynchronize()
{
    Q_D(Bluetooth);
    d->initializeCancellingTimer();
    return d->m_CancellingTimer->isActive();
}

void Bluetooth::cancelSynchronize(BluetoothRecordType type)
{
    Q_D(Bluetooth);
    if (d->m_SyncType == type) {
        qDebug() << __PRETTY_FUNCTION__ << __LINE__;
        d->cancelSync();
    }
}

void Bluetooth::synchronizePhoneBook()
{
    g_Radio->setRadioIdle();
    Q_D(Bluetooth);
    qDebug() << __PRETTY_FUNCTION__ << __LINE__;
    d->cancelSync();
    d->m_PhoneBookList.clear();
    d->syncPhoneBook();
    d->initializeAutoCancelTimer();
    d->m_AutoCancelTimer->start();
}

void Bluetooth::synchronizeOutgoing()
{
    g_Radio->setRadioIdle();
    Q_D(Bluetooth);
    qDebug() << __PRETTY_FUNCTION__ << __LINE__;
    d->cancelSync();
    d->m_OutgoingList.clear();
    d->syncOutgoing();
    d->initializeAutoCancelTimer();
    d->m_AutoCancelTimer->start();
}

void Bluetooth::synchronizeIncoming()
{
    g_Radio->setRadioIdle();
    Q_D(Bluetooth);
    qDebug() << __PRETTY_FUNCTION__ << __LINE__;
    d->cancelSync();
    d->m_IncomingList.clear();
    d->syncIncoming();
    d->initializeAutoCancelTimer();
    d->m_AutoCancelTimer->start();
}

void Bluetooth::synchronizeMissed()
{
    g_Radio->setRadioIdle();
    Q_D(Bluetooth);
    qDebug() << __PRETTY_FUNCTION__ << __LINE__;
    d->cancelSync();
    d->m_MissedList.clear();
    d->syncMissed();
    d->initializeAutoCancelTimer();
    d->m_AutoCancelTimer->start();
}

void Bluetooth::setPincode(const QString &pincode)
{
    Q_D(Bluetooth);
    ArkBlueTooth bluetooth;
    bluetooth.type = ABT_SetPinCode;
    bluetooth.length = pincode.length();
    bluetooth.data = (unsigned char*)pincode.toLocal8Bit().data();
    ArkProtocol protocol;
    protocol.type = AT_Bluetooth;
    protocol.direction = AD_Setter;
    protocol.data = static_cast<void*>(&bluetooth);
    d->onArkProtocol(protocol);
}

void Bluetooth::setLocalDeviceName(const QString &devicename)
{
    Q_D(Bluetooth);
    ArkBlueTooth bluetooth;
    bluetooth.type = ABT_SetLocalName;
    unsigned char data[8] = {0};
    unsigned short int length = (devicename.length() > 8)? 8: devicename.length();
    memcpy(data, devicename.toLocal8Bit().data(), length);
    bluetooth.length = 8;
    bluetooth.data = (unsigned char*)data;
    ArkProtocol protocol;
    protocol.type = AT_Bluetooth;
    protocol.direction = AD_Setter;
    protocol.data = static_cast<void*>(&bluetooth);
    d->onArkProtocol(protocol);
}

void Bluetooth::dialPhone(const QString &phone)
{
    Q_D(Bluetooth);
    ArkBlueTooth bluetooth;
    bluetooth.type = ABT_DialPhone;
    bluetooth.length = phone.length();
    bluetooth.data = (unsigned char*)phone.toLocal8Bit().data();
    ArkProtocol protocol;
    protocol.type = AT_Bluetooth;
    protocol.direction = AD_Setter;
    protocol.data = static_cast<void*>(&bluetooth);
    d->onArkProtocol(protocol);
}

void Bluetooth::dialNumber(const QChar &number)
{
    Q_D(Bluetooth);
    ArkBlueTooth bluetooth;
    bluetooth.type = ABT_DialNumber;
    bluetooth.length = 1;
    unsigned char data[1] = {number.toAscii()};
    bluetooth.data = data;
    ArkProtocol protocol;
    protocol.type = AT_Bluetooth;
    protocol.direction = AD_Setter;
    protocol.data = static_cast<void*>(&bluetooth);
    d->onArkProtocol(protocol);
}

BluetoothVoiceMode Bluetooth::getVoiceMode()
{
    Q_D(Bluetooth);
    return d->m_BluetoothVoiceMode;
}

void Bluetooth::voiceToggleSwitch()
{
    Q_D(Bluetooth);
    ArkBlueTooth bluetooth;
    bluetooth.type = ABT_VoiceSwitch;
    bluetooth.length = 0;
    bluetooth.data = NULL;
    ArkProtocol protocol;
    protocol.type = AT_Bluetooth;
    protocol.direction = AD_Setter;
    protocol.data = static_cast<void*>(&bluetooth);
    d->onArkProtocol(protocol);
}

void Bluetooth::redialLastPhone()
{
    Q_D(Bluetooth);
    ArkBlueTooth bluetooth;
    bluetooth.type = ABT_Redial;
    bluetooth.length = 0;
    bluetooth.data = NULL;
    ArkProtocol protocol;
    protocol.type = AT_Bluetooth;
    protocol.direction = AD_Setter;
    protocol.data = static_cast<void*>(&bluetooth);
    d->onArkProtocol(protocol);
}

void Bluetooth::pickupPhone()
{
    Q_D(Bluetooth);
    ArkBlueTooth bluetooth;
    bluetooth.type = ABT_PickUp;
    bluetooth.length = 0;
    bluetooth.data = NULL;
    ArkProtocol protocol;
    protocol.type = AT_Bluetooth;
    protocol.direction = AD_Setter;
    protocol.data = static_cast<void*>(&bluetooth);
    d->onArkProtocol(protocol);
}

void Bluetooth::hanupPhone()
{
    Q_D(Bluetooth);
    ArkBlueTooth bluetooth;
    bluetooth.type = ABT_Hangup;
    bluetooth.length = 0;
    bluetooth.data = NULL;
    ArkProtocol protocol;
    protocol.type = AT_Bluetooth;
    protocol.direction = AD_Setter;
    protocol.data = static_cast<void*>(&bluetooth);
    d->onArkProtocol(protocol);
}

BluetoothPairedMode Bluetooth::getPowerStatus()
{
    Q_D(Bluetooth);
    qDebug() << __PRETTY_FUNCTION__ << d->m_BluetoothPairedMode;
    return d->m_BluetoothPairedMode;
}

void Bluetooth::powerOn()
{
    qDebug() << __PRETTY_FUNCTION__;
    Q_D(Bluetooth);
    ArkBlueTooth bluetooth;
    bluetooth.type = ABT_PairedMode;
    bluetooth.length = 0;
    bluetooth.data = NULL;
    ArkProtocol protocol;
    protocol.type = AT_Bluetooth;
    protocol.direction = AD_Setter;
    protocol.data = static_cast<void*>(&bluetooth);
    d->onArkProtocol(protocol);
}

void Bluetooth::powerOff()
{
    qDebug() << __PRETTY_FUNCTION__;
    Q_D(Bluetooth);
    d->cancelSync();
    ArkBlueTooth bluetooth;
    bluetooth.type = ABT_CancelPairedMode;
    bluetooth.length = 0;
    bluetooth.data = NULL;
    ArkProtocol protocol;
    protocol.type = AT_Bluetooth;
    protocol.direction = AD_Setter;
    protocol.data = static_cast<void*>(&bluetooth);
    d->onArkProtocol(protocol);
}

BluetoothAutoConnect Bluetooth::getAutoConnectStatus()
{
    Q_D(Bluetooth);
    qDebug() << __PRETTY_FUNCTION__ << d->m_BluetoothAutoConnect;
    return d->m_BluetoothAutoConnect;
}

void Bluetooth::autoConnectOn()
{
    Q_D(Bluetooth);
    ArkBlueTooth bluetooth;
    bluetooth.type = ABT_SetAutoConnect;
    bluetooth.length = 0;
    bluetooth.data = NULL;
    ArkProtocol protocol;
    protocol.type = AT_Bluetooth;
    protocol.direction = AD_Setter;
    protocol.data = static_cast<void*>(&bluetooth);
    d->onArkProtocol(protocol);
}

void Bluetooth::autoConnectOff()
{
    Q_D(Bluetooth);
    ArkBlueTooth bluetooth;
    bluetooth.type = ABT_CancelAutoconnect;
    bluetooth.length = 0;
    bluetooth.data = NULL;
    ArkProtocol protocol;
    protocol.type = AT_Bluetooth;
    protocol.direction = AD_Setter;
    protocol.data = static_cast<void*>(&bluetooth);
    d->onArkProtocol(protocol);
}

BluetoothAutoAnswer Bluetooth::getAutoAnswer()
{
    Q_D(Bluetooth);
    qDebug() << __PRETTY_FUNCTION__ << d->m_BluetoothAutoAnswer;
    return d->m_BluetoothAutoAnswer;
}

void Bluetooth::autoAnswerOn()
{
    Q_D(Bluetooth);
    ArkBlueTooth bluetooth;
    bluetooth.type = ABT_SetAutoAnswer;
    bluetooth.length = 0;
    bluetooth.data = NULL;
    ArkProtocol protocol;
    protocol.type = AT_Bluetooth;
    protocol.direction = AD_Setter;
    protocol.data = static_cast<void*>(&bluetooth);
    d->onArkProtocol(protocol);
}

void Bluetooth::autoAnswerOff()
{
    Q_D(Bluetooth);
    ArkBlueTooth bluetooth;
    bluetooth.type = ABT_CancelAutoAnswer;
    bluetooth.length = 0;
    bluetooth.data = NULL;
    ArkProtocol protocol;
    protocol.type = AT_Bluetooth;
    protocol.direction = AD_Setter;
    protocol.data = static_cast<void*>(&bluetooth);
    d->onArkProtocol(protocol);
}

void Bluetooth::connectRemoteDevice(const unsigned short index)
{
    Q_D(Bluetooth);
    if (BPM_PairedMode == d->m_BluetoothPairedMode) {
        ArkBlueTooth bluetooth;
        bluetooth.type = ABT_Connect;
        unsigned char data = index;
        bluetooth.data = &data;
        d->m_RemoteDeviceName = d->m_PairedMap.value(index);
        bluetooth.length = 1;
        ArkProtocol protocol;
        protocol.type = AT_Bluetooth;
        protocol.direction = AD_Setter;
        protocol.data = static_cast<void*>(&bluetooth);
        d->onArkProtocol(protocol);
    }
}

void Bluetooth::onArkProtocol(const ArkProtocolWrapper &protocol)
{
    if ((AT_Bluetooth == protocol.type)
            && (NULL != protocol.data)) {
        struct ArkBlueTooth* bluetooth = arkProtocolWrapperToArkBluetooth(protocol);
        if (NULL != bluetooth) {
            int i = 0;
            int len = bluetooth->length;
            Q_D(Bluetooth);
            if (d->m_TrusteeshipNativeBluetooth) {
                switch (bluetooth->type) {
                case ABT_GetStatus: {
                    if (NULL != bluetooth->data) {
                        switch (*bluetooth->data) {
                        case ABS_IDLE: {
                            //                        if (NULL != d->m_AutoAnswerTimer) {
                            //                            d->m_AutoAnswerTimer->stop();
                            //                        }
                            if (NULL != d->m_AutoCancelTimer) {
                                d->m_AutoCancelTimer->stop();
                            }
                            d->m_Incoming = false;
                            d->m_Outcoming = false;
                            d->m_BluetoothConnectStatus = BCS_Idle;
                            d->m_MusicName = QString();
                            d->m_BluetoothPlayerStatus = BPS_Pause;
                            onMusicStatusChange(d->m_MusicName, d->m_BluetoothPlayerStatus);
                            d->m_LastNumber = QString();
                            d->m_PhoneBookList.clear();
                            d->m_OutgoingList.clear();
                            d->m_IncomingList.clear();
                            d->m_MissedList.clear();
                            //                            if (NULL != d->m_ForcePlayTimer) {
                            //                                d->m_ForcePlayTimer->stop();
                            //                            }
                            //                            if (MI_Mute == g_Audio->getMute()) {
                            //                                g_Audio->requestSpeaker(SSI_PowerOn);
                            //                                g_Audio->requestMute(MI_Mute);
                            //                            }
                            qDebug() << __PRETTY_FUNCTION__ << __LINE__;
                            //                            d->cancelSync();
                            //                            if (AS_BluetoothPhone == g_Audio->getAudioSource()) {
                            //                                g_Audio->releaseAudioSource(AS_BluetoothPhone);
                            //                                g_Radio->initialize();
                            //                            }
                            //                            if (AS_BluetoothMusic == g_Audio->getAudioSource()) {
                            //                                g_Audio->releaseAudioSource(AS_BluetoothMusic);
                            //                                g_Radio->initialize();
                            //                            }
                            onConnectStatusChange(d->m_BluetoothConnectStatus);
                            break;
                        }
                        case ABS_CONNECTING: {
                            d->m_BluetoothConnectStatus = BCS_Connecting;
                            onConnectStatusChange(d->m_BluetoothConnectStatus);
                            break;
                        }
                        case ABS_CONNECTED: {
                            //                            if (MI_Mute == g_Audio->getMute()) {
                            //                                g_Audio->requestSpeaker(SSI_PowerOn);
                            //                                g_Audio->requestMute(MI_Mute);
                            //                            }
                            //                            if (AS_BluetoothPhone == g_Audio->getAudioSource()) {
                            //                                g_Audio->releaseAudioSource(AS_BluetoothPhone);
                            //                            }
                            d->m_Incoming = false;
                            d->m_Outcoming = false;
                            d->m_BluetoothConnectStatus = BCS_Connected;
                            onConnectStatusChange(d->m_BluetoothConnectStatus);
                            break;
                        }
                        default: {
                            break;
                        }
                        }
                    }
                    break;
                }
                case ABT_GetRemoteAddr: {
                    if (NULL != bluetooth->data) {
                        d->m_RemoteAddress.clear();
                        for (i = 0; i < bluetooth->length; ++i) {
                            d->m_RemoteAddress.append(QChar(*(bluetooth->data + i)));
                            if ((i > 0)
                                    && (1 == (i % 2))
                                    && ((bluetooth->length - 1) > i)) {
                                d->m_RemoteAddress.append(QChar(':'));
                            }
                        }
                        qDebug() << __PRETTY_FUNCTION__ << __LINE__ << d->m_RemoteAddress;
                    }
                    break;
                }
                case ABT_GetRemoteName: {
                    if (NULL != bluetooth->data) {
                        d->m_RemoteDeviceName.clear();
                        char name[16];
                        for (i = 0; i < 15 && len > 0; i++,len--)
                            name[i] = bluetooth->data[i];
                        name[i] = 0;
                        printf("bt remote device name is %s.\n", name);
                        d->m_RemoteDeviceName = QString::fromLocal8Bit((const char *)bluetooth->data, bluetooth->length);
                        if (BCS_Connected == d->m_BluetoothConnectStatus) {
                            onRemoteDeviceNameChange(d->m_RemoteDeviceName);
                        }
                    }
                    break;
                }
                case ABT_PairedListName: {
                    qDebug() << "ABT_PairedListName" << bluetooth->length;
                    if (0 != bluetooth->length) {
                        if (NULL != bluetooth->data) {
                            if (1 == bluetooth->length) {
                                if (0xFF == bluetooth->data[0]) {
                                    d->m_PairedMap.clear();
                                }
                            } else {
                                for (int i = 1; i < bluetooth->length; ++i) {
                                    if (0xFF == bluetooth->data[i]) {
                                        int index = int(bluetooth->data[i - 1]);
                                        QString name = QString::fromLocal8Bit((const char *)bluetooth->data + i * sizeof(unsigned char) + 1, bluetooth->length - i - 1);
                                        d->m_PairedMap.insert(index, name);
                                        break;
                                    }
                                }
                            }
                        }
                    } else {
                        onPairedListChange(d->m_PairedMap);
                    }
                    break;
                }
                default: {
                    break;
                }
                }
            } else {
                switch (bluetooth->type) {
                case ABT_GetStatus: {
                    if (NULL != bluetooth->data) {
                        switch (*bluetooth->data) {
                        case ABS_IDLE: {
                            //                        if (NULL != d->m_AutoAnswerTimer) {
                            //                            d->m_AutoAnswerTimer->stop();
                            //                        }
                            if (NULL != d->m_AutoCancelTimer) {
                                d->m_AutoCancelTimer->stop();
                            }
                            if (NULL != d->m_CancellingTimer) {
                                d->m_CancellingTimer->stop();
                            }
                            d->m_Incoming = false;
                            d->m_Outcoming = false;
                            d->m_BluetoothConnectStatus = BCS_Idle;
                            d->m_MusicName = QString();
                            d->m_BluetoothPlayerStatus = BPS_Pause;
                            onMusicStatusChange(d->m_MusicName, d->m_BluetoothPlayerStatus);
                            d->m_LastNumber = QString();
                            d->m_PhoneBookList.clear();
                            d->m_OutgoingList.clear();
                            d->m_IncomingList.clear();
                            d->m_MissedList.clear();
                            //                            if (NULL != d->m_ForcePlayTimer) {
                            //                                d->m_ForcePlayTimer->stop();
                            //                            }
                            if (MI_Mute == g_Audio->getMute()) {
                                g_Audio->requestSpeaker(SSI_PowerOn);
                                g_Audio->requestMute(MI_Mute);
                            }
                            qDebug() << __PRETTY_FUNCTION__ << __LINE__;
                            d->cancelSync();
                            if (AS_BluetoothPhone == g_Audio->getAudioSource()) {
                                g_Audio->releaseAudioSource(AS_BluetoothPhone);
                                g_Radio->initialize();
                            }
                            if (AS_BluetoothMusic == g_Audio->getAudioSource()) {
                                g_Audio->releaseAudioSource(AS_BluetoothMusic);
                                g_Radio->initialize();
                            }
                            onConnectStatusChange(d->m_BluetoothConnectStatus);
                            break;
                        }
                        case ABS_CONNECTING: {
                            d->m_BluetoothConnectStatus = BCS_Connecting;
                            onConnectStatusChange(d->m_BluetoothConnectStatus);
                            break;
                        }
                        case ABS_CONNECTED: {
                            if (MI_Mute == g_Audio->getMute()) {
                                g_Audio->requestSpeaker(SSI_PowerOn);
                                g_Audio->requestMute(MI_Mute);
                            }
                            if (AS_BluetoothPhone == g_Audio->getAudioSource()) {
                                g_Audio->releaseAudioSource(AS_BluetoothPhone);
                            }
                            d->m_Incoming = false;
                            d->m_Outcoming = false;
                            d->m_BluetoothConnectStatus = BCS_Connected;
                            onConnectStatusChange(d->m_BluetoothConnectStatus);
                            break;
                        }
                        case ABS_OUTGOING: {
                            //                            if (NULL != d->m_ForcePlayTimer) {
                            //                                d->m_ForcePlayTimer->stop();
                            //                            }
                            d->m_Incoming = false;
                            d->m_Outcoming = true;
                            d->m_BluetoothConnectStatus = BCS_Outgoing;
                            onConnectStatusChange(d->m_BluetoothConnectStatus);
                            qDebug() << __PRETTY_FUNCTION__ << __LINE__;
                            g_Audio->requestAudioSource(AS_BluetoothPhone);
                            break;
                        }
                        case ABS_INCOMING: {
                            //                            if (NULL != d->m_ForcePlayTimer) {
                            //                                d->m_ForcePlayTimer->stop();
                            //                            }
                            d->m_Incoming = true;
                            d->m_Outcoming = false;
                            d->m_BluetoothConnectStatus = BCS_Incoming;
                            onConnectStatusChange(d->m_BluetoothConnectStatus);
                            qDebug() << __PRETTY_FUNCTION__ << __LINE__;
                            g_Audio->requestAudioSource(AS_BluetoothPhone);
                            break;
                        }
                        case ABS_TALKING: {
                            //                            if (NULL != d->m_ForcePlayTimer) {
                            //                                d->m_ForcePlayTimer->stop();
                            //                            }
                            d->m_BluetoothConnectStatus = BCS_Talking;
                            onConnectStatusChange(d->m_BluetoothConnectStatus);
                            qDebug() << __PRETTY_FUNCTION__ << __LINE__;
                            g_Audio->requestAudioSource(AS_BluetoothPhone);
                            break;
                        }
                        default: {
                            return;
                            break;
                        }
                        }
                        switch (d->m_BluetoothConnectStatus) {
                        case BCS_Outgoing:
                        case BCS_Incoming:
                        case BCS_Talking: {
                            if (DST_Off == g_Setting->getDisplayScreenType()) {
                                usleep(66666);
                                system("echo 30 > /sys/devices/platform/pwm-backlight/backlight/pwm-backlight/brightness");
                            }
                            if (MI_Mute == g_Audio->getMute()) {
                                g_Audio->requestSpeaker(SSI_PowerOn);
                                g_Audio->requestMute(MI_Unmute);
                            }
                            break;
                        }
                        default: {
                            if (DST_Off == g_Setting->getDisplayScreenType()) {
                                if (ACS_ReversingOff == g_Setting->getReversing()) {
                                    system("echo 100 > /sys/devices/platform/pwm-backlight/backlight/pwm-backlight/brightness");
                                }
                            }
                            break;
                        }
                        }
                    }
                    break;
                }
                case ABT_Hangup: {
                    qDebug() << "ABT_Hangup" << d->m_BluetoothConnectStatus << d->m_LastNumber;
                    qDebug() << __PRETTY_FUNCTION__ << __LINE__;
                    if ((BCS_Outgoing == d->m_BluetoothConnectStatus)
                            || ((BCS_Talking == d->m_BluetoothConnectStatus)
                                && (d->m_Outcoming))) {
                        d->m_Outcoming = false;
                        qDebug() << __PRETTY_FUNCTION__ << __LINE__;
                        if (QString() != d->m_LastNumber) {
                            qDebug() << __PRETTY_FUNCTION__ << __LINE__;
                            struct ContactInfo info;
                            info.phone = d->m_LastNumber;
                            //                            for (auto& iter : d->m_OutgoingList) {
                            for (QList<struct ContactInfo>::iterator iter = d->m_OutgoingList.begin();
                                 iter != d->m_OutgoingList.end(); ++iter) {
                                if (d->m_LastNumber == iter->phone) {
                                    info.name = iter->name;
                                    break;
                                }
                            }
                            //                            for (auto& iter : d->m_PhoneBookList) {
                            for (QList<struct ContactInfo>::iterator iter = d->m_PhoneBookList.begin();
                                 iter != d->m_PhoneBookList.end(); ++iter) {
                                if (d->m_LastNumber == iter->phone) {
                                    info.name = iter->name;
                                    break;
                                }
                            }
                            d->m_OutgoingList.prepend(info);
                            d->m_LastNumber = QString();
                            qDebug() << __PRETTY_FUNCTION__ << __LINE__;
                            onRecordChange(BRT_Outgoing, d->m_OutgoingList);
                        }
                    } else if (BCS_Incoming == d->m_BluetoothConnectStatus) {
                        qDebug() << __PRETTY_FUNCTION__ << __LINE__;
                        if (QString() != d->m_LastNumber) {
                            qDebug() << __PRETTY_FUNCTION__ << __LINE__;
                            struct ContactInfo info;
                            info.phone = d->m_LastNumber;
                            info.name.clear();
                            if (info.name.isEmpty()) {
                                //                                for (auto& iter : d->m_MissedList) {
                                for (QList<struct ContactInfo>::iterator iter = d->m_MissedList.begin();
                                     iter != d->m_MissedList.end(); ++iter) {
                                    if (d->m_LastNumber == iter->phone) {
                                        info.name = iter->name;
                                        break;
                                    }
                                }
                            }
                            if (info.name.isEmpty()) {
                                //                                for (auto& iter : d->m_PhoneBookList) {
                                for (QList<struct ContactInfo>::iterator iter = d->m_PhoneBookList.begin();
                                     iter != d->m_PhoneBookList.end(); ++iter) {
                                    if (d->m_LastNumber == iter->phone) {
                                        info.name = iter->name;
                                        break;
                                    }
                                }
                            }
                            d->m_MissedList.prepend(info);
                            d->m_LastNumber = QString();
                            qDebug() << __PRETTY_FUNCTION__ << __LINE__ << d->m_MissedList.size();
                            onRecordChange(BRT_Missed, d->m_MissedList);
                        }
                    } else if ((BCS_Talking == d->m_BluetoothConnectStatus)
                               && (d->m_Incoming)) {
                        d->m_Incoming = false;
                        qDebug() << __PRETTY_FUNCTION__ << __LINE__ << d->m_LastNumber;
                        if (QString() != d->m_LastNumber) {
                            qDebug() << __PRETTY_FUNCTION__ << __LINE__;
                            struct ContactInfo info;
                            info.phone = d->m_LastNumber;
                            info.name.clear();
                            if (info.name.isEmpty()) {
                                //                                for (auto& iter : d->m_IncomingList) {
                                for (QList<struct ContactInfo>::iterator iter = d->m_IncomingList.begin();
                                     iter != d->m_IncomingList.end(); ++iter) {
                                    if (d->m_LastNumber == iter->phone) {
                                        info.name = iter->name;
                                        break;
                                    }
                                }
                            }
                            if (info.name.isEmpty()) {
                                //                                for (auto& iter : d->m_PhoneBookList) {
                                for (QList<struct ContactInfo>::iterator iter = d->m_PhoneBookList.begin();
                                     iter != d->m_PhoneBookList.end(); ++iter) {
                                    if (d->m_LastNumber == iter->phone) {
                                        info.name = iter->name;
                                        break;
                                    }
                                }
                            }
                            d->m_IncomingList.prepend(info);
                            d->m_LastNumber = QString();
                            qDebug() << __PRETTY_FUNCTION__ << __LINE__;
                            emit onRecordChange(BRT_Incoming, d->m_IncomingList);
                            qDebug() << __PRETTY_FUNCTION__ << __LINE__;
                        }
                    }
                    if (BPS_Play == d->m_BluetoothPlayerStatus) {
                        //                        d->m_BluetoothPlayerStatus = BPS_Play;
                        //                        d->initializeForcePlayTimer();
                        //                        d->m_ForcePlayTimer->start();
                        Q_D(Bluetooth);
                        ArkBlueTooth bluetooth;
                        bluetooth.type = ABT_MusicPlay;
                        bluetooth.length = 0;
                        bluetooth.data = NULL;
                        ArkProtocol protocol;
                        protocol.type = AT_Bluetooth;
                        protocol.direction = AD_Setter;
                        protocol.data = static_cast<void*>(&bluetooth);
                        d->onArkProtocol(protocol);
                    }
#if 0
                    if (MI_Mute == g_Audio->getMute()) {
                        g_Audio->requestSpeaker(SSI_PowerOn);
                        g_Audio->requestMute(MI_Mute);
                    }
                    if (AS_BluetoothPhone == g_Audio->getAudioSource()) {
                        g_Audio->releaseAudioSource(AS_BluetoothPhone);
                    }
#endif
                    break;
                }
                case ABT_VoiceBluetooth: {
                    qDebug() << __PRETTY_FUNCTION__;
                    d->m_BluetoothVoiceMode = BVM_VoiceBluetooth;
                    onVoiceChange(d->m_BluetoothVoiceMode);
                    break;
                }
                case ABT_VoicePhone: {
                    qDebug() << __PRETTY_FUNCTION__;
                    d->m_BluetoothVoiceMode = BVM_VoicePhone;
                    onVoiceChange(d->m_BluetoothVoiceMode);
                    break;
                }
                case ABT_PairedMode: {
                    if (BPM_PairedMode != d->m_BluetoothPairedMode) {
                        d->m_BluetoothPairedMode = BPM_PairedMode;
                        onPowerChange(d->m_BluetoothPairedMode);
                    }
                    break;
                }
                case ABT_CancelPairedMode: {
                    if (BPM_CancelPairedMode != d->m_BluetoothPairedMode) {
                        d->m_BluetoothPairedMode = BPM_CancelPairedMode;
                        onPowerChange(d->m_BluetoothPairedMode);
                    }
                    break;
                }
                case ABT_GetLocalName:
                    if (NULL != bluetooth->data) {
                        char name[16];
                        for (i = 0; i < 15 && len > 0; i++,len--)
                            name[i] = bluetooth->data[i];
                        name[i] = 0;
                        printf("bt local device name is %s.\n", name);
                        d->m_LocalDeviceName = QString::fromLocal8Bit((const char *)bluetooth->data, bluetooth->length);
                        qDebug() << __PRETTY_FUNCTION__ << __LINE__ << d->m_LocalDeviceName;
                        onLocalDeviceNameChange(d->m_LocalDeviceName);
                    }
                    break;
                case ABT_GetRemoteName:
                    if (NULL != bluetooth->data) {
                        d->m_RemoteDeviceName.clear();
                        char name[16];
                        for (i = 0; i < 15 && len > 0; i++,len--)
                            name[i] = bluetooth->data[i];
                        name[i] = 0;
                        printf("bt remote device name is %s.\n", name);
                        d->m_RemoteDeviceName = QString::fromLocal8Bit((const char *)bluetooth->data, bluetooth->length);
                        qDebug() << __PRETTY_FUNCTION__ << __LINE__;
                    }
                    break;
                case ABT_GetPinCode:
                    if (NULL != bluetooth->data) {
                        char pincode[5];
                        for (i = 0; i < 4 && len > 0; i++,len--)
                            pincode[i] = bluetooth->data[i];
                        pincode[i] = 0;
                        printf("bt pincode is %s.\n", pincode);
                        d->m_Pincode = QString::fromLocal8Bit((const char *)bluetooth->data, bluetooth->length);
                        qDebug() << "write pincode";
                        QFile pinfile(QString("/tmp/bluetoothpin"));
                        if (pinfile.open(QFile::WriteOnly)) {
                            pinfile.write(d->m_Pincode.toLocal8Bit().data(), d->m_Pincode.length());
                            pinfile.flush();
                            pinfile.close();
                        }
                        onPinCodeChange(d->m_Pincode);
                        qDebug() << __PRETTY_FUNCTION__ << __LINE__ << d->m_Pincode;
                    }
                    break;
                case ABT_GetLocalAddr: {
                    if (NULL != bluetooth->data) {
                        d->m_LocalAddress.clear();
                        for (i = 0; i < bluetooth->length; ++i) {
                            d->m_LocalAddress.append(QChar(*(bluetooth->data + i)));
                            if ((i > 0)
                                    && (1 == (i % 2))
                                    && ((bluetooth->length - 1) > i)) {
                                d->m_LocalAddress.append(QChar(':'));
                            }
                        }
                        qDebug() << __PRETTY_FUNCTION__ << "write address";
                        QFile addressfile(QString("/tmp/bluetoothaddress"));
                        if (addressfile.open(QFile::WriteOnly)) {
                            addressfile.write(d->m_LocalAddress.toLocal8Bit().data(), d->m_LocalAddress.length());
                            addressfile.flush();
                            addressfile.close();
                        }
                    }
                    break;
                }
                case ABT_GetRemoteAddr: {
                    if (NULL != bluetooth->data) {
                        d->m_RemoteAddress.clear();
                        for (i = 0; i < bluetooth->length; ++i) {
                            d->m_RemoteAddress.append(QChar(*(bluetooth->data + i)));
                            if ((i > 0)
                                    && (1 == (i % 2))
                                    && ((bluetooth->length - 1) > i)) {
                                d->m_RemoteAddress.append(QChar(':'));
                            }
                        }
                        qDebug() << __PRETTY_FUNCTION__ << __LINE__ << d->m_RemoteAddress;
                    }
                    break;
                }
                case ABT_GetOutgoingNumber: {
                    if (NULL != bluetooth->data) {
                        char number[32];
                        for (i = 0; i < 31 && len > 0; i++,len--)
                            number[i] = bluetooth->data[i];
                        number[i] = 0;
                        d->m_LastNumber = QString::fromLocal8Bit((const char *)bluetooth->data, bluetooth->length);
                        QString name("");
                        for(int i = 0; i < d->m_PhoneBookList.size(); ++i) {
                            if (d->m_PhoneBookList.at(i).phone == d->m_LastNumber) {
                                name = d->m_PhoneBookList.at(i).name;
                                break;
                            }
                        }
                        printf("ABT_GetOutgoingNumber number now is %s.\n", number);
                        onDialInfo(BRT_Undefine, name, d->m_LastNumber);
                    }
                    break;
                }
                case ABT_GetIncomingNumber: {
                    if (NULL != bluetooth->data) {
                        char number[32];
                        for (i = 0; i < 31 && len > 0; i++,len--)
                            number[i] = bluetooth->data[i];
                        number[i] = 0;
                        d->m_LastNumber = QString::fromLocal8Bit((const char *)bluetooth->data, bluetooth->length);
                        QString name("");
                        for(int i = 0; i < d->m_PhoneBookList.size(); ++i) {
                            if (d->m_PhoneBookList.at(i).phone == d->m_LastNumber) {
                                name = d->m_PhoneBookList.at(i).name;
                                break;
                            }
                        }
                        printf("ABT_GetIncomingNumber number now is %s.\n", number);
                        onDialInfo(BRT_Undefine, name, d->m_LastNumber);
                    }
                    break;
                }
                case ABT_GetPhoneNumberNow:
                    if (NULL != bluetooth->data) {
                        char number[32];
                        for (i = 0; i < 31 && len > 0; i++,len--)
                            number[i] = bluetooth->data[i];
                        number[i] = 0;
                        QString phone = QString::fromLocal8Bit((const char *)bluetooth->data, bluetooth->length);
                        QString name("");
                        for(int i = 0; i < d->m_PhoneBookList.size(); ++i) {
                            if (d->m_PhoneBookList.at(i).phone == phone) {
                                name = d->m_PhoneBookList.at(i).name;
                                break;
                            }
                        }
                        printf("phone number now is %s.\n", number);
                    }
                    break;
                case ABT_GetPhoneBookEntry: {
                    if (NULL != d->m_AutoCancelTimer) {
                        d->m_AutoCancelTimer->start();
                    }
                    printf("get a phonebook enrty.\n");
                    if (BRT_PhoneBook == d->m_SyncType) {
                        for (int i = 0; i < bluetooth->length; ++i) {
                            if (0xFF == bluetooth->data[i]) {
                                struct ContactInfo info;
                                info.name = QString::fromLocal8Bit((const char *)bluetooth->data, i);
                                info.phone = QString::fromLocal8Bit((const char *)bluetooth->data + i * sizeof(unsigned char) + 1, bluetooth->length - i - 1);
                                d->m_PhoneBookList.append(info);
                                onRecordCountChange(BRT_PhoneBook, d->m_PhoneBookList.count());
                                break;
                            }
                        }
                    }
                    break;
                }
                case ABT_GetReceiveRecordEntry: {
                    if (NULL != d->m_AutoCancelTimer) {
                        d->m_AutoCancelTimer->start();
                    }
                    printf("get a reveive record enrty.\n");
                    if (BRT_Incoming == d->m_SyncType) {
                        for (int i = 0; i < bluetooth->length; ++i) {
                            if (0xFF == bluetooth->data[i]) {
                                struct ContactInfo info;
                                info.name = QString::fromLocal8Bit((const char *)bluetooth->data, i);
                                info.phone = QString::fromLocal8Bit((const char *)bluetooth->data + i * sizeof(unsigned char) + 1, bluetooth->length - i - 1);
                                d->m_IncomingList.append(info);
                                onRecordCountChange(BRT_Incoming, d->m_IncomingList.count());
                                break;
                            }
                        }
                    }
                    break;
                }
                case ABT_GetDialedRecordEntry: {
                    if (NULL != d->m_AutoCancelTimer) {
                        d->m_AutoCancelTimer->start();
                    }
                    printf("get a dialed record enrty.\n");
                    if (BRT_Outgoing == d->m_SyncType) {
                        for (int i = 0; i < bluetooth->length; ++i) {
                            if (0xFF == bluetooth->data[i]) {
                                struct ContactInfo info;
                                info.name = QString::fromLocal8Bit((const char *)bluetooth->data, i);
                                info.phone = QString::fromLocal8Bit((const char *)bluetooth->data + i * sizeof(unsigned char) + 1, bluetooth->length - i - 1);
                                d->m_OutgoingList.append(info);
                                onRecordCountChange(BRT_Outgoing, d->m_OutgoingList.count());
                                break;
                            }
                        }
                    }
                    break;
                }
                case ABT_GetMissRecordEntry: {
                    if (NULL != d->m_AutoCancelTimer) {
                        d->m_AutoCancelTimer->start();
                    }
                    printf("get a miss record enrty.\n");
                    if (BRT_Missed == d->m_SyncType) {
                        for (int i = 0; i < bluetooth->length; ++i) {
                            if (0xFF == bluetooth->data[i]) {
                                struct ContactInfo info;
                                info.name = QString::fromLocal8Bit((const char *)bluetooth->data, i);
                                info.phone = QString::fromLocal8Bit((const char *)bluetooth->data + i * sizeof(unsigned char) + 1, bluetooth->length - i - 1);
                                d->m_MissedList.append(info);
                                onRecordCountChange(BRT_Missed, d->m_MissedList.count());
                                break;
                            }
                        }
                    }
                    break;
                }
                case ABT_SyncPhoneBook: {
                    onSyncChange(BRT_PhoneBook, BSS_Synchronizing);
                    break;
                }
                case ABT_SyncReceiveRecord: {
                    onSyncChange(BRT_Incoming, BSS_Synchronizing);
                    break;
                }
                case ABT_SyncDialedRecord: {
                    onSyncChange(BRT_Outgoing, BSS_Synchronizing);
                    break;
                }
                case ABT_SyncMissRecord: {
                    onSyncChange(BRT_Missed, BSS_Synchronizing);
                    break;
                }
                case ABT_SyncPhoneBookCancelling: {
                    d->initializeCancellingTimer();
                    d->m_CancellingTimer->start();
                    onSyncChange(BRT_PhoneBook, BSS_Cacelling);
                    break;
                }
                case ABT_SyncReceiveRecordCancelling: {
                    d->initializeCancellingTimer();
                    d->m_CancellingTimer->start();
                    onSyncChange(BRT_Incoming, BSS_Cacelling);
                    break;
                }
                case ABT_SyncDialedRecordCancelling: {
                    d->initializeCancellingTimer();
                    d->m_CancellingTimer->start();
                    onSyncChange(BRT_Outgoing, BSS_Cacelling);
                    break;
                }
                case ABT_SyncMissRecordCancelling: {
                    d->initializeCancellingTimer();
                    d->m_CancellingTimer->start();
                    onSyncChange(BRT_Missed, BSS_Cacelling);
                    break;
                }
                case ABT_SyncPhoneBookEnd: {
                    qDebug() << "ABT_SyncPhoneBookEnd";
                    if (NULL != d->m_AutoCancelTimer) {
                        d->m_AutoCancelTimer->stop();
                    }
                    if (NULL != d->m_CancellingTimer) {
                        d->m_CancellingTimer->stop();
                    }
                    if (BRT_PhoneBook == d->m_SyncType) {
                        d->m_SyncType = BRT_Undefine;
                    }
                    onRecordChange(BRT_PhoneBook, d->m_PhoneBookList);
                    if (BPS_Play == d->m_BluetoothPlayerStatus) {
                        Q_D(Bluetooth);
                        ArkBlueTooth bluetooth;
                        bluetooth.type = ABT_MusicPlay;
                        bluetooth.length = 0;
                        bluetooth.data = NULL;
                        ArkProtocol protocol;
                        protocol.type = AT_Bluetooth;
                        protocol.direction = AD_Setter;
                        protocol.data = static_cast<void*>(&bluetooth);
                        d->onArkProtocol(protocol);
                    }
                    break;
                }
                case ABT_SyncReceiveRecordEnd: {
                    qDebug() << "ABT_SyncReceiveRecordEnd";
                    if (NULL != d->m_AutoCancelTimer) {
                        d->m_AutoCancelTimer->stop();
                    }
                    if (NULL != d->m_CancellingTimer) {
                        d->m_CancellingTimer->stop();
                    }
                    if (BRT_Incoming == d->m_SyncType) {
                        d->m_SyncType = BRT_Undefine;
                    }
                    onRecordChange(BRT_Incoming, d->m_IncomingList);
                    if (BPS_Play == d->m_BluetoothPlayerStatus) {
                        Q_D(Bluetooth);
                        ArkBlueTooth bluetooth;
                        bluetooth.type = ABT_MusicPlay;
                        bluetooth.length = 0;
                        bluetooth.data = NULL;
                        ArkProtocol protocol;
                        protocol.type = AT_Bluetooth;
                        protocol.direction = AD_Setter;
                        protocol.data = static_cast<void*>(&bluetooth);
                        d->onArkProtocol(protocol);
                    }
                    break;
                }
                case ABT_SyncDialedRecordEnd: {
                    qDebug() << "ABT_SyncDialedRecordEnd";
                    if (NULL != d->m_AutoCancelTimer) {
                        d->m_AutoCancelTimer->stop();
                    }
                    if (NULL != d->m_CancellingTimer) {
                        d->m_CancellingTimer->stop();
                    }
                    if (BRT_Outgoing == d->m_SyncType) {
                        d->m_SyncType = BRT_Undefine;
                    }
                    onRecordChange(BRT_Outgoing, d->m_OutgoingList);
                    if (BPS_Play == d->m_BluetoothPlayerStatus) {
                        Q_D(Bluetooth);
                        ArkBlueTooth bluetooth;
                        bluetooth.type = ABT_MusicPlay;
                        bluetooth.length = 0;
                        bluetooth.data = NULL;
                        ArkProtocol protocol;
                        protocol.type = AT_Bluetooth;
                        protocol.direction = AD_Setter;
                        protocol.data = static_cast<void*>(&bluetooth);
                        d->onArkProtocol(protocol);
                    }
                    break;
                }
                case ABT_SyncMissRecordEnd: {
                    qDebug() << "ABT_SyncMissRecordEnd";
                    if (NULL != d->m_AutoCancelTimer) {
                        d->m_AutoCancelTimer->stop();
                    }
                    if (NULL != d->m_CancellingTimer) {
                        d->m_CancellingTimer->stop();
                    }
                    if (BRT_Missed == d->m_SyncType) {
                        d->m_SyncType = BRT_Undefine;
                    }
                    onRecordChange(BRT_Missed, d->m_MissedList);
                    if (BPS_Play == d->m_BluetoothPlayerStatus) {
                        Q_D(Bluetooth);
                        ArkBlueTooth bluetooth;
                        bluetooth.type = ABT_MusicPlay;
                        bluetooth.length = 0;
                        bluetooth.data = NULL;
                        ArkProtocol protocol;
                        protocol.type = AT_Bluetooth;
                        protocol.direction = AD_Setter;
                        protocol.data = static_cast<void*>(&bluetooth);
                        d->onArkProtocol(protocol);
                    }
                    break;
                }
                case ABT_GetAutoAnswer:
                    printf("bt auto answer.\n");
                    if (BAA_AutoAnswer != d->m_BluetoothAutoAnswer) {
                        d->m_BluetoothAutoAnswer = BAA_AutoAnswer;
                        onAutoAnswerChange(d->m_BluetoothAutoAnswer);
                    }
                    break;
                case ABT_GetNotAutoAnswer:
                    printf("bt not auto answer.\n");
                    if (BAA_CancelAutoAnswer != d->m_BluetoothAutoAnswer) {
                        d->m_BluetoothAutoAnswer = BAA_CancelAutoAnswer;
                        onAutoAnswerChange(d->m_BluetoothAutoAnswer);
                    }
                    break;
                case ABT_GetAutoConnect:
                    printf("bt auto connect.\n");
                    if (BAC_AutoConnect != d->m_BluetoothAutoConnect) {
                        d->m_BluetoothAutoConnect = BAC_AutoConnect;
                        onAutoConnectChange(d->m_BluetoothAutoConnect);
                    }
                    break;
                case ABT_GetNotAutoConnect:
                    printf("bt not auto connect.\n");
                    if (BAC_CancelAutoConnect != d->m_BluetoothAutoConnect) {
                        d->m_BluetoothAutoConnect = BAC_CancelAutoConnect;
                        onAutoConnectChange(d->m_BluetoothAutoConnect);
                    }
                    break;
                case ABT_GetA2DPConnect: {
                    printf("a2dp is connected.\n");
                    if (BCS_Connected == d->m_BluetoothConnectStatus) {
                        d->m_BluetoothPlayerStatus = BPS_Pause;
                        onMusicStatusChange(d->m_MusicName, d->m_BluetoothPlayerStatus);
                    }
                    //music pause
                    break;
                }
                case ABT_GetMusicPlaying: {
                    printf("bt music is playing.\n");
                    //                    if (NULL != d->m_ForcePlayTimer) {
                    //                        d->m_ForcePlayTimer->stop();
                    //                    }
                    if (BCS_Connected == d->m_BluetoothConnectStatus) {
                        d->m_BluetoothPlayerStatus = BPS_Play;
                        onMusicStatusChange(d->m_MusicName, d->m_BluetoothPlayerStatus);
                    }
                    //music play
                    break;
                }
                case ABT_MusicName: {
                    d->m_MusicName = QString::fromLocal8Bit((const char *)bluetooth->data, bluetooth->length);
                    onMusicStatusChange(d->m_MusicName, d->m_BluetoothPlayerStatus);
                    break;
                }
                case ABT_GetVoiceConnect: {
                    printf("bt voice is connected.\n");
                    onVoiceChange(d->m_BluetoothVoiceMode);
                    break;
                }
                case ABT_GetVoiceDisconnect:
                    printf("bt voice is disconnected.\n");
                    break;
                case ABT_GetA2DPDisconnect:
                    printf("a2dp is disconnected.\n");
                    break;
                case ABT_PairedListName: {
                    qDebug() << "ABT_PairedListName" << bluetooth->length;
                    if (0 != bluetooth->length) {
                        if (NULL != bluetooth->data) {
                            if (1 == bluetooth->length) {
                                if (0xFF == bluetooth->data[0]) {
                                    d->m_PairedMap.clear();
                                }
                            } else {
                                for (int i = 1; i < bluetooth->length; ++i) {
                                    if (0xFF == bluetooth->data[i]) {
                                        int index = int(bluetooth->data[i - 1]);
                                        QString name = QString::fromLocal8Bit((const char *)bluetooth->data + i * sizeof(unsigned char) + 1, bluetooth->length - i - 1);
                                        d->m_PairedMap.insert(index, name);
                                        break;
                                    }
                                }
                            }
                        }
                    } else {
                        onPairedListChange(d->m_PairedMap);
                    }
                    break;
                }
                default: {
                    break;
                }
                }
            }
        }
    }
}

void Bluetooth::onHolderChange(const AudioSource oldHolder, const AudioSource newHolder)
{
    switch (newHolder) {
    case AS_Aux: {
        if (AS_BluetoothMusic == oldHolder) {
            Q_D(Bluetooth);
            ArkBlueTooth bluetooth;
            bluetooth.type = ABT_MusicPause;
            bluetooth.length = 0;
            bluetooth.data = NULL;
            ArkProtocol protocol;
            protocol.type = AT_Bluetooth;
            protocol.direction = AD_Setter;
            protocol.data = static_cast<void*>(&bluetooth);
            d->onArkProtocol(protocol);
            //            if (NULL != d->m_ForcePlayTimer) {
            //                d->m_ForcePlayTimer->stop();
            //            }
        }
        break;
    }
    case AS_BluetoothMusic: {
        if (AS_Aux == oldHolder) {
            if (g_Audio->requestAudioSource(AS_BluetoothMusic)) {
                Q_D(Bluetooth);
                ArkBlueTooth bluetooth;
                bluetooth.type = ABT_MusicPlay;
                bluetooth.length = 0;
                bluetooth.data = NULL;
                ArkProtocol protocol;
                protocol.type = AT_Bluetooth;
                protocol.direction = AD_Setter;
                protocol.data = static_cast<void*>(&bluetooth);
                d->onArkProtocol(protocol);
                //                d->initializeForcePlayTimer();
                //                d->m_ForcePlayTimer->start();
            }
        }
        break;
    }
    default: {
        break;
    }
    }
}

void Bluetooth::onTimeout()
{
    qDebug() << __PRETTY_FUNCTION__ << __LINE__;
    QTimer* ptr = static_cast<QTimer*>(sender());
    Q_D(Bluetooth);
    /*if (ptr == d->m_AutoAnswerTimer) {
    } else */if (ptr == d->m_AutoCancelTimer) {
        qDebug() << __PRETTY_FUNCTION__ << __LINE__;
        d->cancelSync();
    }/* else if (ptr == d->m_ForcePlayTimer) {
        Q_D(Bluetooth);
        ArkBlueTooth bluetooth;
        bluetooth.type = ABT_MusicPlay;
        bluetooth.length = 0;
        bluetooth.data = NULL;
        ArkProtocol protocol;
        protocol.type = AT_Bluetooth;
        protocol.direction = AD_Setter;
        protocol.data = static_cast<void*>(&bluetooth);
        d->onArkProtocol(protocol);
    }*/ else if (ptr == d->m_CancellingTimer) {
        qDebug() << __PRETTY_FUNCTION__ << __LINE__;
        onSyncChange(d->m_SyncType, BSS_Undefine);
        d->m_SyncType = BRT_Undefine;
        //        QList<struct ContactInfo> tmp;
        //        onRecordChange(d->m_SyncType, tmp);
    }
}

void Bluetooth::setInnerBluetooth(const ArkBlueTooth* bluetooth)
{
    if (NULL != bluetooth) {
        Q_D(Bluetooth);
        switch (bluetooth->type) {
        case ABT_GetLocalName: {
            if (NULL != bluetooth->data) {
                d->m_LocalDeviceName = QString::fromLocal8Bit((const char *)bluetooth->data, bluetooth->length);
            }
            break;
        }
        case ABT_GetLocalAddr: {
            if (NULL != bluetooth->data) {
                d->m_LocalAddress.clear();
                for (int i = 0; i < bluetooth->length; ++i) {
                    d->m_LocalAddress.append(QChar(*(bluetooth->data + i)));
                    if ((i > 0)
                            && (1 == (i % 2))
                            && ((bluetooth->length - 1) > i)) {
                        d->m_LocalAddress.append(QChar(':'));
                    }
                }
            }
            break;
        }
        case ABT_GetPinCode: {
            if (NULL != bluetooth->data) {
                d->m_Pincode = QString::fromLocal8Bit((const char *)bluetooth->data, bluetooth->length);
            }
            break;
        }
        case ABT_PairedMode: {
            d->m_BluetoothPairedMode = BPM_PairedMode;
            break;
        }
        case ABT_CancelPairedMode: {
            d->m_BluetoothPairedMode = BPM_CancelPairedMode;
            break;
        }
        case ABT_GetAutoConnect: {
            d->m_BluetoothAutoConnect = BAC_AutoConnect;
            break;
        }
        case ABT_GetNotAutoConnect: {
            d->m_BluetoothAutoConnect = BAC_CancelAutoConnect;
            break;
        }
        case ABT_GetAutoAnswer: {
            d->m_BluetoothAutoAnswer = BAA_AutoAnswer;
            break;
        }
        case ABT_GetNotAutoAnswer: {
            d->m_BluetoothAutoAnswer = BAA_CancelAutoAnswer;
            break;
        }
        case ABT_GetRemoteName: {
            if (NULL != bluetooth->data) {
                d->m_RemoteDeviceName = QString::fromLocal8Bit((const char *)bluetooth->data, bluetooth->length);
            }
            break;
        }
        case ABT_GetRemoteAddr: {
            if (NULL != bluetooth->data) {
                d->m_RemoteAddress.clear();
                for (int i = 0; i < bluetooth->length; ++i) {
                    d->m_RemoteAddress.append(QChar(*(bluetooth->data + i)));
                    if ((i > 0)
                            && (1 == (i % 2))
                            && ((bluetooth->length - 1) > i)) {
                        d->m_RemoteAddress.append(QChar(':'));
                    }
                }
            }
            break;
        }
        case ABT_GetStatus: {
            if (NULL != bluetooth->data) {
                switch (*bluetooth->data) {
                case ABS_IDLE: {
                    d->m_BluetoothConnectStatus = BCS_Idle;
                    break;
                }
                case ABS_CONNECTING: {
                    d->m_BluetoothConnectStatus = BCS_Connecting;
                    break;
                }
                case ABS_CONNECTED: {
                    d->m_BluetoothConnectStatus = BCS_Connected;
                    break;
                }
                case ABS_OUTGOING: {
                    d->m_BluetoothConnectStatus = BCS_Outgoing;
                    break;
                }
                case ABS_INCOMING: {
                    d->m_BluetoothConnectStatus = BCS_Incoming;
                    break;
                }
                case ABS_TALKING: {
                    d->m_BluetoothConnectStatus = BCS_Talking;
                    break;
                }
                default: {
                    break;
                }
                }
            }
            break;
        }
        case ABT_PairedListName: {
            if (0 != bluetooth->length) {
                if (NULL != bluetooth->data) {
                    if (1 != bluetooth->length) {
                        for (int i = 1; i < bluetooth->length; ++i) {
                            if (0xFF == bluetooth->data[i]) {
                                int index = int(bluetooth->data[i - 1]);
                                QString name = QString::fromLocal8Bit((const char *)bluetooth->data + i * sizeof(unsigned char) + 1, bluetooth->length - i - 1);
                                d->m_PairedMap.insert(index, name);
                                break;
                            }
                        }
                    }
                }
            }
            break;
        }
        default: {
            break;
        }
        }
    }
}

Bluetooth::Bluetooth(QObject *parent)
    : QObject(parent)
    , d_ptr(new BluetoothPrivate(this))
{
}

Bluetooth::~Bluetooth()
{
    if (NULL != d_ptr) {
        delete d_ptr;
    }
}

BluetoothPrivate::BluetoothPrivate(Bluetooth *parent)
    : q_ptr(parent)
{
    m_BluetoothConnectStatus = BCS_Idle;
    m_Incoming = false;
    m_Outcoming = false;
    m_BluetoothPlayerStatus = BPS_Pause;
    m_AutoCancelTimer = NULL;
    m_BluetoothVoiceMode = BVM_VoiceBluetooth;
    m_BluetoothPairedMode = BPM_PairedMode;
    m_BluetoothAutoConnect = BAC_AutoConnect;
    m_BluetoothAutoAnswer = BAA_AutoAnswer;
    m_SyncType = BRT_Undefine;
    m_TrusteeshipNativeBluetooth = false;
    m_CancellingTimer = NULL;
    connectAllSlots();
}

BluetoothPrivate::~BluetoothPrivate()
{
}

void BluetoothPrivate::connectAllSlots()
{
    Q_Q(Bluetooth);
    connectSignalAndSlotByNamesake(g_Setting, q, ARKRECEIVER(onArkProtocol(const ArkProtocolWrapper&)));
    connectSignalAndSlotByNamesake(g_Audio, q, ARKRECEIVER(onHolderChange(const int, const int)));
}

void BluetoothPrivate::onArkProtocol(const ArkProtocol &protocol)
{
    g_Setting->setArkProtocol(ArkProtocolWrapper(protocol));
}

void BluetoothPrivate::cancelSync()
{
    qDebug() << __PRETTY_FUNCTION__ << __LINE__;
    if (BRT_Undefine != m_SyncType) {
        qDebug() << __PRETTY_FUNCTION__ << __LINE__;
        ArkBlueTooth bluetooth;
        bluetooth.type = ABT_CancelSync;
        bluetooth.length = 0;
        bluetooth.data = NULL;
        ArkProtocol protocol;
        protocol.type = AT_Bluetooth;
        protocol.direction = AD_Setter;
        protocol.data = static_cast<void*>(&bluetooth);
        onArkProtocol(protocol);
        if (NULL != m_AutoCancelTimer) {
            m_AutoCancelTimer->stop();
        }
    }
}

void BluetoothPrivate::syncPhoneBook()
{
    qDebug() << __PRETTY_FUNCTION__;
    m_SyncType = BRT_PhoneBook;
    ArkBlueTooth bluetooth;
    bluetooth.type = ABT_SyncPhoneBook;
    bluetooth.length = 0;
    bluetooth.data = NULL;
    ArkProtocol protocol;
    protocol.type = AT_Bluetooth;
    protocol.direction = AD_Setter;
    protocol.data = static_cast<void*>(&bluetooth);
    onArkProtocol(protocol);
}

void BluetoothPrivate::syncOutgoing()
{
    qDebug() << __PRETTY_FUNCTION__;
    m_SyncType = BRT_Outgoing;
    ArkBlueTooth bluetooth;
    bluetooth.type = ABT_SyncDialedRecord;
    bluetooth.length = 0;
    bluetooth.data = NULL;
    ArkProtocol protocol;
    protocol.type = AT_Bluetooth;
    protocol.direction = AD_Setter;
    protocol.data = static_cast<void*>(&bluetooth);
    onArkProtocol(protocol);
}

void BluetoothPrivate::syncIncoming()
{
    qDebug() << __PRETTY_FUNCTION__;
    m_SyncType = BRT_Incoming;
    ArkBlueTooth bluetooth;
    bluetooth.type = ABT_SyncReceiveRecord;
    bluetooth.length = 0;
    bluetooth.data = NULL;
    ArkProtocol protocol;
    protocol.type = AT_Bluetooth;
    protocol.direction = AD_Setter;
    protocol.data = static_cast<void*>(&bluetooth);
    onArkProtocol(protocol);
}

void BluetoothPrivate::syncMissed()
{
    qDebug() << __PRETTY_FUNCTION__;
    m_SyncType = BRT_Missed;
    ArkBlueTooth bluetooth;
    bluetooth.type = ABT_SyncMissRecord;
    bluetooth.length = 0;
    bluetooth.data = NULL;
    ArkProtocol protocol;
    protocol.type = AT_Bluetooth;
    protocol.direction = AD_Setter;
    protocol.data = static_cast<void*>(&bluetooth);
    onArkProtocol(protocol);
}

//void BluetoothPrivate::initializeForcePlayTimer()
//{
//    if (NULL == m_ForcePlayTimer) {
//        Q_Q(Bluetooth);
//        m_ForcePlayTimer = new QTimer(q);
//        m_ForcePlayTimer->setSingleShot(false);
//        m_ForcePlayTimer->setInterval(500);
//        QObject::connect(m_ForcePlayTimer, ARKSENDER(timeout()),
//                         q,                ARKRECEIVER(onTimeout()));
//    }
//}

//void BluetoothPrivate::initializeAutoAnswerTimer()
//{
//    if (NULL == m_AutoAnswerTimer) {
//        Q_Q(Bluetooth);
//        m_AutoAnswerTimer = new QTimer(q);
//        m_AutoAnswerTimer->setInterval(10 * 1000);
//        m_AutoAnswerTimer->setSingleShot(true);
//        QObject::connect(m_AutoAnswerTimer, ARKSENDER(timeout()),
//                         q,                 ARKRECEIVER(onTimeout()));
//    }
//}

void BluetoothPrivate::initializeAutoCancelTimer()
{
    if (NULL == m_AutoCancelTimer) {
        Q_Q(Bluetooth);
        m_AutoCancelTimer = new QTimer(q);
        m_AutoCancelTimer->setInterval(10 * 1000);
        m_AutoCancelTimer->setSingleShot(true);
        QObject::connect(m_AutoCancelTimer, ARKSENDER(timeout()),
                         q,                 ARKRECEIVER(onTimeout()));
    }
}

void BluetoothPrivate::initializeCancellingTimer()
{
    if (NULL == m_CancellingTimer) {
        Q_Q(Bluetooth);
        m_CancellingTimer = new QTimer(q);
        m_CancellingTimer->setInterval(3333);
        m_CancellingTimer->setSingleShot(true);
        QObject::connect(m_CancellingTimer, ARKSENDER(timeout()),
                         q,                 ARKRECEIVER(onTimeout()));
    }
}
