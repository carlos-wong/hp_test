#include "Radio.h"
#include "Setting.h"
#include "AutoConnect.h"
#include "Audio.h"
#include "Bluetooth.h"
#include "UserInterfaceUtility.h"
#include <unistd.h>
#include <QDebug>
#include <QDomDocument>

class RadioPrivate {
public:
    explicit RadioPrivate(Radio* parent = NULL);
    ~RadioPrivate();
    void connectAllSlots();
    void onArkProtocol(const ArkProtocol& protocol);
private:
    Radio* const q_ptr;
    Q_DECLARE_PUBLIC(Radio)
    RadioStereoType m_RadioUsedStereoType;
    RadioStereoType m_RadioListenStereoType;
    RadioLocType m_RadioLocType;
    RadioBandType m_RadioBandType;
    unsigned short m_Index;
    unsigned short m_Frequency;
    QList<unsigned short> m_RadioList;
    unsigned short m_MinFrequency;
    unsigned short m_MaxFrequency;
    unsigned short m_Step;
    RadioWorkType m_RadioWorkType;
    QByteArray m_ByteArray;
    SpeakerSoundItem m_SpeakerSoundItem;
    bool m_StartComplete;
};

bool Radio::isStartComplete()
{
    Q_D(Radio);
    return d->m_StartComplete;
}

void Radio::reset()
{
}

static void sourceswitch(void)
{
    struct ArkControl control;
    memset((void*)&control, 0, sizeof(struct ArkControl));
    struct ArkProtocol protocol;
    memset((void*)&protocol, 0, sizeof(struct ArkProtocol));
    protocol.type = AT_Control;
    protocol.direction = AD_Setter;
    protocol.data = &control;
    control.type = ACLT_RequestSourceSwitch;
    unsigned char data[1] = {0x04};
    control.data = data;
    control.length = 1;
    g_Setting->setArkProtocol(ArkProtocolWrapper(protocol));
}

void Radio::initialize()
{
    if (g_Audio->requestAudioSource(AS_Radio)) {
        Q_D(Radio);
        sourceswitch();
    }
}

void Radio::toggleStereo()
{
    Q_D(Radio);
    ArkRadio radio;
    radio.type = ART_KEY_StereoStateChange;
    radio.length = 0;
    radio.data = NULL;
    ArkProtocol protocol;
    protocol.type = AT_Radio;
    protocol.direction = AD_Setter;
    protocol.data = static_cast<void*>(&radio);
    d->onArkProtocol(protocol);
}

void Radio::toggleLoc()
{
    Q_D(Radio);
    ArkRadio radio;
    radio.type = ART_KEY_LOC;
    radio.length = 0;
    radio.data = NULL;
    ArkProtocol protocol;
    protocol.type = AT_Radio;
    protocol.direction = AD_Setter;
    protocol.data = static_cast<void*>(&radio);
    d->onArkProtocol(protocol);
}

void Radio::toggleBand()
{
    qDebug() << __PRETTY_FUNCTION__;
    Q_D(Radio);
    ArkRadio radio;
    radio.type = ART_KEY_Band;
    radio.length = 0;
    radio.data = NULL;
    ArkProtocol protocol;
    protocol.type = AT_Radio;
    protocol.direction = AD_Setter;
    protocol.data = static_cast<void*>(&radio);
    d->onArkProtocol(protocol);
}

void Radio::setCurrentBandTurnAdd()
{
    Q_D(Radio);
    ArkRadio radio;
    radio.type = ART_KEY_TuningDown;
    radio.length = 0;
    radio.data = NULL;
    ArkProtocol protocol;
    protocol.type = AT_Radio;
    protocol.direction = AD_Setter;
    protocol.data = static_cast<void*>(&radio);
    d->onArkProtocol(protocol);
}

void Radio::setCurrentBandTurnSub()
{
    Q_D(Radio);
    ArkRadio radio;
    radio.type = ART_KEY_TuningUp;
    radio.length = 0;
    radio.data = NULL;
    ArkProtocol protocol;
    protocol.type = AT_Radio;
    protocol.direction = AD_Setter;
    protocol.data = static_cast<void*>(&radio);
    d->onArkProtocol(protocol);
}

void Radio::setRadioIdle()
{
    Q_D(Radio);
    if (RWT_Idle != d->m_RadioWorkType) {
        ArkRadio radio;
        switch (d->m_RadioBandType) {
        case RBT_FM1:
        case RBT_FM2:
        case RBT_FM3: {
            radio.type = ART_KEY_FMFreq;
            break;
        }
        case RBT_AM1:
        case RBT_AM2: {
            radio.type = ART_KEY_AMFreq;
            break;
        }
        default: {
            return;
            break;
        }
        }
        unsigned char data[2] = {0x00, 0x00};
        radio.length = 2;
        radio.data = data;
        ArkProtocol protocol;
        protocol.type = AT_Radio;
        protocol.direction = AD_Setter;
        protocol.data = static_cast<void*>(&radio);
        d->onArkProtocol(protocol);
        usleep(111 * 1111);
    }
}

void Radio::setCurrentBandFrequency(const unsigned short frequency)
{
    Q_D(Radio);
    ArkRadio radio;
    switch (d->m_RadioBandType) {
    case RBT_FM1:
    case RBT_FM2:
    case RBT_FM3: {
        radio.type = ART_KEY_FMFreq;
        break;
    }
    case RBT_AM1:
    case RBT_AM2: {
        radio.type = ART_KEY_AMFreq;
        break;
    }
    default: {
        return;
        break;
    }
    }
    unsigned char data[2] = {0};
    data[0] = frequency >> 8;
    data[1] = frequency & 0xFF;
    radio.length = 2;
    radio.data = data;
    ArkProtocol protocol;
    protocol.type = AT_Radio;
    protocol.direction = AD_Setter;
    protocol.data = static_cast<void*>(&radio);
    d->onArkProtocol(protocol);
}

void Radio::setCurrentBandIndex(const unsigned short index)
{
    if ((index >= 1)
            && (index <= 6)) {
        Q_D(Radio);
        ArkRadio radio;
        radio.type = ArkRadioType(ART_KEY + index);
        radio.length = 0;
        radio.data = NULL;
        ArkProtocol protocol;
        protocol.type = AT_Radio;
        protocol.direction = AD_Setter;
        protocol.data = static_cast<void*>(&radio);
        d->onArkProtocol(protocol);
    }
}

void Radio::setSearchPreviousStation()
{
    Q_D(Radio);
    ArkRadio radio;
    radio.type = ART_KEY_SeekUp;
    radio.length = 0;
    radio.data = NULL;
    ArkProtocol protocol;
    protocol.type = AT_Radio;
    protocol.direction = AD_Setter;
    protocol.data = static_cast<void*>(&radio);
    d->onArkProtocol(protocol);
}

void Radio::setSearchNextStation()
{
    Q_D(Radio);
    ArkRadio radio;
    radio.type = ART_KEY_SeekDown;
    radio.length = 0;
    radio.data = NULL;
    ArkProtocol protocol;
    protocol.type = AT_Radio;
    protocol.direction = AD_Setter;
    protocol.data = static_cast<void*>(&radio);
    d->onArkProtocol(protocol);
}

void Radio::setCurrentBandSearch()
{
    Q_D(Radio);
    ArkRadio radio;
    radio.type = ART_KEY_AutoScan;
    radio.length = 0;
    radio.data = NULL;
    ArkProtocol protocol;
    protocol.type = AT_Radio;
    protocol.direction = AD_Setter;
    protocol.data = static_cast<void*>(&radio);
    d->onArkProtocol(protocol);
}

void Radio::setCurrentBandBrowse()
{
    Q_D(Radio);
    ArkRadio radio;
    radio.type = ART_KEY_PS;
    radio.length = 0;
    radio.data = NULL;
    ArkProtocol protocol;
    protocol.type = AT_Radio;
    protocol.direction = AD_Setter;
    protocol.data = static_cast<void*>(&radio);
    d->onArkProtocol(protocol);
}

void Radio::setCurrentBandFrequencyToIndex(const unsigned short index)
{
    Q_D(Radio);
    ArkRadio radio;
    radio.type = ART_KEY_PresetSave;
    unsigned char data = index;
    radio.data = &data;
    radio.length = 1;
    ArkProtocol protocol;
    protocol.type = AT_Radio;
    protocol.direction = AD_Setter;
    protocol.data = static_cast<void*>(&radio);
    d->onArkProtocol(protocol);
}

int Radio::getCurrendBandIndex()
{
    Q_D(Radio);
    return d->m_Index;
}

int Radio::getCurrendBandFrequency()
{
    Q_D(Radio);
    return d->m_Frequency;
}

RadioWorkType Radio::getWorkType()
{
    Q_D(Radio);
    return d->m_RadioWorkType;
}

RadioBandType Radio::getCurrentBand()
{
    Q_D(Radio);
    return d->m_RadioBandType;
}

RadioStereoType Radio::getUsedStereoType()
{
    Q_D(Radio);
    return d->m_RadioUsedStereoType;
}

RadioStereoType Radio::getListenStereoType()
{
    Q_D(Radio);
    return d->m_RadioListenStereoType;
}

RadioLocType Radio::getLocType()
{
    Q_D(Radio);
    return d->m_RadioLocType;
}

unsigned short Radio::getCurrentBandMin()
{
    Q_D(Radio);
    return d->m_MinFrequency;
}

unsigned short Radio::getCurrentBandMax()
{
    Q_D(Radio);
    return d->m_MaxFrequency;
}

unsigned short Radio::getCurrentBandStep()
{
    Q_D(Radio);
    return d->m_Step;
}

const QList<unsigned short> &Radio::getCurrentBandList()
{
    Q_D(Radio);
    return d->m_RadioList;
}

void Radio::setRadioSmeter(QByteArray &byteArray)
{
    Q_D(Radio);
    ArkRadio radio;
    radio.type = ART_KEY_Smeter;
    radio.length = (unsigned char)byteArray.length();
    radio.data = (unsigned char*)byteArray.data();
    ArkProtocol protocol;
    protocol.type = AT_Radio;
    protocol.direction = AD_Setter;
    protocol.data = static_cast<void*>(&radio);
    d->onArkProtocol(protocol);
}

QByteArray &Radio::getRadioSmeter()
{
    Q_D(Radio);
    return d->m_ByteArray;
}

void Radio::onArkProtocol(const ArkProtocolWrapper &protocol)
{
    if ((AT_Radio == protocol.type)
            && (NULL != protocol.data)) {
        struct ArkRadio* radio = arkProtocolWrapperToArkRadio(protocol);
        if (NULL != radio) {
            Q_D(Radio);
            switch (radio->type) {
            case ART_KEY_Smeter: {
                if (NULL != radio->data) {
                    d->m_ByteArray.clear();
                    d->m_ByteArray.resize(radio->length);
                    for (int i = 0; i < radio->length; ++i) {
                        d->m_ByteArray[i] = radio->data[i];
                    }
                    onRadioSmeter(d->m_ByteArray);
                }
                break;
            }
            case ART_GetMinFreq:
                if (NULL != radio->data) {
                    printf("receive radio current min frequence is %d.\n", (radio->data[0] << 8) | radio->data[1]);
                    d->m_MinFrequency = (radio->data[0] << 8) | radio->data[1];
                }
                break;
            case ART_GetMaxFreq:
                if (NULL != radio->data) {
                    printf("receive radio current max frequence is %d.\n", (radio->data[0] << 8) | radio->data[1]);
                    d->m_MaxFrequency = (radio->data[0] << 8) | radio->data[1];
                }
                break;
            case ART_GetGranularity:
                if (NULL != radio->data) {
                    printf("receive radio current granularity is %d.\n", radio->data[0]);
                    d->m_Step = radio->data[0];
                }
                break;
            case ART_GetSignalIntensity:
                if (NULL != radio->data) {
                    //                    printf("receive radio current sigal intensity is %d.\n", radio->data[0]);
                }
                break;
            case ART_GetStatus:
                if (NULL != radio->data) {
                    switch (static_cast<ArkRadioStatus>(*radio->data)) {
                    case ARS_IDLE:
                    case ARS_INIT: {
                        d->m_RadioWorkType = RWT_Idle;
                        break;
                    }
                    case ARS_SEEK_UP: {
                        d->m_RadioWorkType = RWT_SeekUp;
                        break;
                    }
                    case ARS_SEEK_DOWN: {
                        d->m_RadioWorkType = RWT_SeekDown;
                        break;
                    }
                    case ARS_AUTOSTORE: {
                        d->m_RadioWorkType = RWT_AutoStore;
                        break;
                    }
                    case ARS_BROWSE_PRESTORE: {
                        d->m_RadioWorkType = RWT_BrowseStore;
                        break;
                    }
                    default: {
                        qDebug() << __PRETTY_FUNCTION__ << __LINE__ << "undefine radio status type!!!!!!!!!!!!";
                        break;
                    }
                    }
                    switch (d->m_RadioWorkType) {
                    case RWT_SeekUp:
                    case RWT_SeekDown:
                    case RWT_AutoStore: {
                        if (BRT_Undefine != g_Bluetooth->getSynchronizeType()) {
                            g_Bluetooth->cancelSynchronize(g_Bluetooth->getSynchronizeType());
                        }
                        //                        if (MI_Unmute == g_Audio->getMute()) {
                        //                            g_Audio->requestMute(MI_Mute);
                        //                            d->m_SpeakerSoundItem = SSI_PowerOff;
                        g_Audio->requestSpeaker(SSI_PowerOff);
                        //                        }
                        break;
                    }
                    default: {
                        qDebug() << __PRETTY_FUNCTION__ << __LINE__ << g_Audio->getMute();
                        //                        if (MI_Unmute == g_Audio->getMute()) {
                        //                            d->m_SpeakerSoundItem = SSI_Undefine;
                        g_Audio->requestSpeaker(SSI_PowerOn);
                        //                            g_Audio->requestMute(MI_Unmute);
                        //                        }
                        break;
                    }
                    }
                    if (RWT_Idle == d->m_RadioWorkType) {
                        onRadioInfoChange(d->m_RadioBandType, d->m_Frequency, d->m_Index, d->m_MinFrequency, d->m_MaxFrequency, d->m_Step);
                    }
                    printf("radio current status is %d.\n", int(*radio->data));
                }
                break;
            case ART_GetCurfreqMono:
                printf("radio current frequence is mono.\n");
                d->m_RadioListenStereoType = RST_Mono;
                onListenStereoChange(d->m_RadioListenStereoType);
                break;
            case ART_GetCurfreqStereo:
                printf("radio current frequence is stereo.\n");
                d->m_RadioListenStereoType = RST_Stereo;
                onListenStereoChange(d->m_RadioListenStereoType);
                break;
            case ART_GetUseMono:
                printf("radio use mono.\n");
                d->m_RadioUsedStereoType = RST_Mono;
                onUsedStereoChange(d->m_RadioUsedStereoType);
                break;
            case ART_GetUseStereo:
                printf("radio use stereo.\n");
                d->m_RadioUsedStereoType = RST_Stereo;
                onUsedStereoChange(d->m_RadioUsedStereoType);
                break;
            case ART_GetAutoScan:
                //                printf("radio scan mode is auto.\n");
                break;
            case ART_GetManualScan:
                //                printf("radio scan mode is manual.\n");
                break;
            case ART_GetLOC:
                //                printf("radio is LOC now.\n");
                d->m_RadioLocType = RLT_LocalSensitivityOn;
                onLocChange(RLT_LocalSensitivityOn);
                break;
            case ART_GetDX:
                //                printf("radio is DX now.\n");
                d->m_RadioLocType = RLT_LocalSensitivityOff;
                onLocChange(RLT_LocalSensitivityOff);
                break;
            case ART_GetBand:
                if (NULL != radio->data) {
                    switch (*radio->data) {
                    case 0x00: {
                        d->m_RadioBandType = RBT_FM1;
                        break;
                    }
                    case 0x01: {
                        d->m_RadioBandType = RBT_FM2;
                        break;
                    }
                    case 0x02: {
                        d->m_RadioBandType = RBT_FM3;
                        break;
                    }
                    case 0x03: {
                        d->m_RadioBandType = RBT_AM1;
                        break;
                    }
                    case 0x04: {
                        d->m_RadioBandType = RBT_AM2;
                        break;
                    }
                    default: {
                        break;
                    }
                    }
                    qDebug() << __PRETTY_FUNCTION__ << __LINE__ << "m_Private->m_RadioBandType" << d->m_RadioBandType;
                    //                    printf("radio current band is %d.\n", *radio->data);
                }
                break;
            case ART_GetPreset:
                if (NULL != radio->data) {
                    d->m_Index = short(*radio->data);
                    printf("radio preset channel is %d.\n", *radio->data);
                }
                break;
            case ART_GetFreq:
                if (NULL != radio->data) {
                    if (NULL != radio->data) {
                        d->m_Frequency = (radio->data[0] << 8) | radio->data[1];
                        onRadioInfoChange(d->m_RadioBandType, d->m_Frequency, d->m_Index, d->m_MinFrequency, d->m_MaxFrequency, d->m_Step);
                        //                        printf("radio preset frequence is %d.\n", (radio->data[0] << 8) | radio->data[1]);
                        //                        printf("radio preset frequence0 is %d.\n", radio->data[0]);
                        //                        printf("radio preset frequence1 is %d.\n", radio->data[1]);
                    }
                }
                break;
            case ART_GetPresetList:
                qDebug() << __PRETTY_FUNCTION__ << __LINE__;
                if (NULL != radio->data) {
                    //                    QDomDocument domDocument;
                    //                    domDocument.clear();
                    //                    domDocument.appendChild(domDocument.createElement(QString("Radio")));
                    //                    QDomElement root = domDocument.firstChildElement(QString("Radio"));
                    //                    QDomElement list;
                    //                    list = domDocument.createElement(QString("List"));
                    //                    root.appendChild(list);
                    //                    d->m_RadioList.clear();
                    //                    QDomElement info;
                    d->m_RadioList.clear();
                    for (int i = 0; i < RADIO_PRESET_NUM; ++i) {
                        //                        info = domDocument.createElement(QString("Index:" + QString::number(i)));
                        //                        list.appendChild(info);
                        //                        info.appendChild(domDocument.createTextNode(QString::number((radio->data[2*i] << 8) | radio->data[2*i + 1])));
                        d->m_RadioList.append((radio->data[2 * i] << 8) | (radio->data[2 * i + 1]));
                    }
                    //                    d->m_RadioList = domDocument.toString();
                    onRadioListChange(d->m_RadioList);
                    qDebug() << __PRETTY_FUNCTION__ << __LINE__;
                    UserInterfaceUtility::elapsed(QString(__PRETTY_FUNCTION__) + QString::number(__LINE__));
                }
                break;
            }
        }
    }
}

void Radio::onHolderChange(const AudioSource oldHolder, const AudioSource newHolder)
{
    qDebug() << __PRETTY_FUNCTION__ << __LINE__;
    Q_D(Radio);
    if (AS_Radio == newHolder) {
        initialize();
        //        switch (d->m_ArkRadioStatus) {
        //        case ARS_SEEK_UP:
        //        case ARS_SEEK_DOWN:
        //        case ARS_AUTOSTORE: {
        //            if (MI_Unmute == g_Audio->getMute()) {
        //                g_Audio->requestMute(MI_Mute);
        //                d->m_SpeakerSoundItem = SSI_PowerOff;
        //                g_Audio->requestSpeaker(SSI_PowerOff);
        //            }
        //            break;
        //        }
        //        default: {
        //            if (MI_Unmute == g_Audio->getMute()) {
        //                d->m_SpeakerSoundItem = SSI_Undefine;
        //                g_Audio->requestSpeaker(SSI_PowerOn);
        //                g_Audio->requestMute(MI_Unmute);
        //            }
        //            break;
        //        }
        //        }
    }/* else {
        if (SSI_PowerOff == d->m_SpeakerSoundItem) {
            d->m_SpeakerSoundItem = SSI_Undefine;
            g_Audio->requestSpeaker(SSI_PowerOn);
            if (MI_Unmute == g_Audio->getMute()) {
                g_Audio->requestMute(MI_Unmute);
            }
        }
    }*/
}

void Radio::setInnerRadio(const ArkRadio* radio)
{
    if (NULL != radio) {
        Q_D(Radio);
        switch (radio->type) {
        case ART_KEY_Smeter: {
            if (NULL != radio->data) {
                d->m_ByteArray.clear();
                d->m_ByteArray.resize(radio->length);
                for (int i = 0; i < radio->length; ++i) {
                    d->m_ByteArray[i] = radio->data[i];
                }
            }
            break;
        }
        case ART_GetCurfreqMono: {
            d->m_RadioListenStereoType = RST_Mono;
            break;
        }
        case ART_GetCurfreqStereo: {
            d->m_RadioListenStereoType = RST_Stereo;
            break;
        }
        case ART_GetUseMono: {
            d->m_RadioUsedStereoType = RST_Mono;
            break;
        }
        case ART_GetUseStereo: {
            d->m_RadioUsedStereoType = RST_Stereo;
            break;
        }
        case ART_GetLOC: {
            d->m_RadioLocType = RLT_LocalSensitivityOn;
            break;
        }
        case ART_GetDX: {
            d->m_RadioLocType = RLT_LocalSensitivityOff;
            break;
        }
        case ART_GetMinFreq: {
            if (NULL != radio->data) {
                d->m_MinFrequency = (radio->data[0] << 8) | radio->data[1];
            }
            break;
        }
        case ART_GetMaxFreq: {
            if (NULL != radio->data) {
                d->m_MaxFrequency = (radio->data[0] << 8) | radio->data[1];
            }
            break;
        }
        case ART_GetGranularity: {
            if (NULL != radio->data) {
                d->m_Step = radio->data[0];
            }
            break;
        }
        case ART_GetBand: {
            if (NULL != radio->data) {
                switch (*radio->data) {
                case 0x00: {
                    d->m_RadioBandType = RBT_FM1;
                    break;
                }
                case 0x01: {
                    d->m_RadioBandType = RBT_FM2;
                    break;
                }
                case 0x02: {
                    d->m_RadioBandType = RBT_FM3;
                    break;
                }
                case 0x03: {
                    d->m_RadioBandType = RBT_AM1;
                    break;
                }
                case 0x04: {
                    d->m_RadioBandType = RBT_AM2;
                    break;
                }
                default: {
                    break;
                }
                }
            }
            break;
        }
        case ART_GetPresetList: {
            if (NULL != radio->data) {
                d->m_RadioList.clear();
                for (int i = 0; i < RADIO_PRESET_NUM; ++i) {
                    d->m_RadioList.append((radio->data[2 * i] << 8) | (radio->data[2 * i + 1]));
                }
            }
            d->m_StartComplete = true;
            startComplete();
            break;
        }
        case ART_GetPreset: {
            if (NULL != radio->data) {
                d->m_Index = short(*radio->data);
            }
            break;
        }
        case ART_GetFreq: {
            if (NULL != radio->data) {
                if (NULL != radio->data) {
                    d->m_Frequency = (radio->data[0] << 8) | radio->data[1];
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

Radio::Radio(QObject* parent)
    : QObject(parent)
    , d_ptr(new RadioPrivate(this))
{
}

Radio::~Radio()
{
    if (NULL != d_ptr) {
        delete d_ptr;
    }
}

RadioPrivate::RadioPrivate(Radio *parent)
    : q_ptr(parent)
{
    m_RadioUsedStereoType = RST_Mono;
    m_RadioListenStereoType = RST_Mono;
    m_RadioLocType = RLT_LocalSensitivityOn;
    m_RadioBandType = RBT_Undefine;
    m_Index = 0;
    m_Frequency = 0;
    m_MinFrequency = 0;
    m_MaxFrequency = 0;
    m_Step = 0;
    m_RadioWorkType = RWT_Idle;
    m_ByteArray;
    m_SpeakerSoundItem = SSI_Undefine;
    m_StartComplete = false;
    connectAllSlots();
}

RadioPrivate::~RadioPrivate()
{
}

void RadioPrivate::connectAllSlots()
{
    Q_Q(Radio);
    connectSignalAndSlotByNamesake(g_Setting, q, ARKRECEIVER(onArkProtocol(const ArkProtocolWrapper&)));
    connectSignalAndSlotByNamesake(g_Audio, q, ARKRECEIVER(onHolderChange(const int, const int)));
}

void RadioPrivate::onArkProtocol(const ArkProtocol &protocol)
{
    g_Setting->setArkProtocol(ArkProtocolWrapper(protocol));
}
