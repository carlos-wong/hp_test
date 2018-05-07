#include "RadioService.h"
#include "AutoConnect.h"
#include <QList>
#include <QString>
#include <QCoreApplication>
#include <QMetaObject>
#include <QDebug>
#include <QTimer>
#include <QTime>
#include <stdio.h>
#include <assert.h>

enum RadioServiceStatus {
    RSS_Undefine = -1,
    RSS_Idle,
    RSS_Search,
    RSS_SearchAdd,
    RSS_SearchSub,
    RSS_Browse,
};

class RadioServicePrivate
{
    Q_DISABLE_COPY(RadioServicePrivate)
public:
    explicit RadioServicePrivate(RadioService* parent);
    ~RadioServicePrivate();
    void initialize();
    void initializeSearchTimer();
    void onArkProtocol(const ArkProtocol& protocol);
    void setFrequency(const RadioBandType type, const unsigned short frequency);
    short getIndexByBand(const RadioBandType type, const unsigned short frequency);
private:
    friend class RadioService;
    RadioService* m_Parent;
    //    QList<QVariant> m_FM1FrequencyList;
    //    QList<QVariant> m_FM2FrequencyList;
    //    QList<QVariant> m_FM3FrequencyList;
    //    QList<QVariant> m_AM1FrequencyList;
    //    QList<QVariant> m_AM2FrequencyList;
    QMap< RadioBandType, QList<QVariant> > m_BandList;
    QMap<RadioBandType, unsigned short> m_BandFrequency;
    QMap<RadioBandType, unsigned short> m_BandStep;
    QMap<RadioBandType, unsigned short> m_MinFrequency;
    QMap<RadioBandType, unsigned short> m_MaxFrequency;
    RadioStereoType m_RadioStereoType;
    RadioLocType m_RadioLocType;
    RadioBandType m_RadioBandType;
    unsigned short m_Index;
    unsigned short m_Frequency;
    QObject* m_Object;
    //    QTimer* m_SearchTimer;
    QTime m_Time;
    RadioServiceStatus m_RadioServiceStatus;
};

RadioService::RadioService(QObject *parent)
    : QObject(parent)
    , m_Private(new RadioServicePrivate(this))
{
}

RadioService::~RadioService()
{
}

void RadioService::setStereo(const RadioStereoType type)
{
}

void RadioService::toggleStereo()
{
    ArkRadio radio;
    radio.type = ART_KEY_StereoStateChange;
    radio.length = 0;
    radio.data = NULL;
    ArkProtocol protocol;
    protocol.type = AT_Radio;
    protocol.direction = AD_Setter;
    protocol.data = static_cast<void*>(&radio);
    m_Private->onArkProtocol(protocol);
}

void RadioService::setLoc(const RadioLocType type)
{
}

void RadioService::toggleLoc()
{
    ArkRadio radio;
    radio.type = ART_KEY_LOC;
    radio.length = 0;
    radio.data = NULL;
    ArkProtocol protocol;
    protocol.type = AT_Radio;
    protocol.direction = AD_Setter;
    protocol.data = static_cast<void*>(&radio);
    m_Private->onArkProtocol(protocol);
}

void RadioService::toggleBand()
{
    ArkRadio radio;
    radio.type = ART_KEY_Band;
    radio.length = 0;
    radio.data = NULL;
    ArkProtocol protocol;
    protocol.type = AT_Radio;
    protocol.direction = AD_Setter;
    protocol.data = static_cast<void*>(&radio);
    m_Private->onArkProtocol(protocol);
}

void RadioService::setCurrentBandTurnAdd()
{
    qWarning() << __PRETTY_FUNCTION__;
    ArkRadio radio;
    radio.type = ART_KEY_TuningDown;
    radio.length = 0;
    radio.data = NULL;
    ArkProtocol protocol;
    protocol.type = AT_Radio;
    protocol.direction = AD_Setter;
    protocol.data = static_cast<void*>(&radio);
    m_Private->onArkProtocol(protocol);
    //    switch (m_Private->m_RadioBandType) {
    //    case RBT_FM1:
    ////    case RBT_FM2:
    ////    case RBT_FM3:
    //    case RBT_AM1:
    ///*    case RBT_AM2:*/ {
    //        unsigned short frequency = m_Private->m_BandFrequency.value(m_Private->m_RadioBandType);
    //        frequency += m_Private->m_BandStep.value(m_Private->m_RadioBandType);
    //        if (frequency > m_Private->m_MaxFrequency.value(m_Private->m_RadioBandType)) {
    //            frequency = m_Private->m_MinFrequency.value(m_Private->m_RadioBandType);
    //        }
    //        m_Private->setFrequency(m_Private->m_RadioBandType, frequency);
    //        break;
    //    }
    //    default: {
    //        return;
    //        break;
    //    }
    //    }
}

void RadioService::setCurrentBandTurnSub()
{
    qWarning() << __PRETTY_FUNCTION__;
    ArkRadio radio;
    radio.type = ART_KEY_TuningDown;
    radio.length = 0;
    radio.data = NULL;
    ArkProtocol protocol;
    protocol.type = AT_Radio;
    protocol.direction = AD_Setter;
    protocol.data = static_cast<void*>(&radio);
    m_Private->onArkProtocol(protocol);
    //    qWarning() << "m_Private->m_RadioBandType" << m_Private->m_RadioBandType;
    //    switch (m_Private->m_RadioBandType) {
    //    case RBT_FM1:
    ////    case RBT_FM2:
    ////    case RBT_FM3:
    //    case RBT_AM1:
    ///*    case RBT_AM2:*/ {
    //        unsigned short frequency = m_Private->m_BandFrequency.value(m_Private->m_RadioBandType);
    //        qWarning() << __PRETTY_FUNCTION__ << "2" << frequency;
    //        frequency -= m_Private->m_BandStep.value(m_Private->m_RadioBandType);
    //        if (frequency < m_Private->m_MinFrequency.value(m_Private->m_RadioBandType)) {
    //            frequency = m_Private->m_MaxFrequency.value(m_Private->m_RadioBandType);
    //        }
    //        qWarning() << __PRETTY_FUNCTION__ << "1" << frequency;
    //        m_Private->setFrequency(m_Private->m_RadioBandType, frequency);
    //        break;
    //    }
    //    default: {
    //        return;
    //        break;
    //    }
    //    }
}

void RadioService::previousFrequencyListIndex(const RadioBandType type)
{
}

void RadioService::previousCurrentBandFrequencyListIndex()
{
}

void RadioService::nextFrequencyListIndex(const RadioBandType type)
{
}

void RadioService::nextCurrentBandFrequencyListIndex()
{
}

void RadioService::setBand(const RadioBandType type)
{
}

void RadioService::setFrequency(const RadioBandType type, const unsigned short frequency)
{
}

void RadioService::setCurrentBandFrequency(const unsigned short frequency)
{
}

void RadioService::setCurrentBandIndex(const unsigned short index)
{
    qWarning() << __PRETTY_FUNCTION__ << index << m_Private->m_RadioBandType;
    if ((index >= 1)
            && (index <= 6)) {
        ArkRadio radio;
        radio.type = ArkRadioType(ART_KEY + index);
        radio.length = 0;
        radio.data = NULL;
        ArkProtocol protocol;
        protocol.type = AT_Radio;
        protocol.direction = AD_Setter;
        protocol.data = static_cast<void*>(&radio);
        m_Private->onArkProtocol(protocol);
    }
    //    if (index < m_Private->m_BandList[m_Private->m_RadioBandType].size()) {
    //        bool flag(true);
    //        unsigned short frequency = m_Private->m_BandList[m_Private->m_RadioBandType].at(index).toInt(&flag);
    //        if (flag) {
    //            m_Private->setFrequency(m_Private->m_RadioBandType, frequency);
    //        }
    //    }
    //    switch (m_Private->m_RadioBandType) {
    //    case RBT_FM1: {
    //        if (index < m_Private->m_FM1FrequencyList.size()) {
    //            bool flag(true);
    //            unsigned short frequency = m_Private->m_FM1FrequencyList.at(index).toInt(&flag);
    //            if (flag) {
    //                m_Private->setFrequency(m_Private->m_RadioBandType, frequency);
    //            }
    //        }
    //        break;
    //    }
    //    case RBT_FM2: {
    //        break;
    //    }
    //    case RBT_FM3: {
    //        break;
    //    }
    //    default: {
    //        return ;
    //        break;
    //    }
    //    }
}

void RadioService::setIndex(const RadioBandType type, const unsigned short index)
{
}

void RadioService::setSearchPreviousStation()
{
    ArkRadio radio;
    radio.type = ART_KEY_SeekUp;
    radio.length = 0;
    radio.data = NULL;
    ArkProtocol protocol;
    protocol.type = AT_Radio;
    protocol.direction = AD_Setter;
    protocol.data = static_cast<void*>(&radio);
    m_Private->onArkProtocol(protocol);
}

void RadioService::setSearchNextStation()
{
    ArkRadio radio;
    radio.type = ART_KEY_SeekDown;
    radio.length = 0;
    radio.data = NULL;
    ArkProtocol protocol;
    protocol.type = AT_Radio;
    protocol.direction = AD_Setter;
    protocol.data = static_cast<void*>(&radio);
    m_Private->onArkProtocol(protocol);
}

void RadioService::setSearch(const RadioBandType type)
{
}

void RadioService::setCurrentBandSearch()
{
    qWarning() << __PRETTY_FUNCTION__;
    ArkRadio radio;
    radio.type = ART_KEY_AutoScan;
    radio.length = 0;
    radio.data = NULL;
    ArkProtocol protocol;
    protocol.type = AT_Radio;
    protocol.direction = AD_Setter;
    protocol.data = static_cast<void*>(&radio);
    m_Private->onArkProtocol(protocol);
    //    m_Private->initializeSearchTimer();
    //    m_Private->m_SearchTimer->start();
}

void RadioService::setBrowse(const RadioBandType type)
{
}

void RadioService::setCurrentBandBrowse()
{
    qWarning() << __PRETTY_FUNCTION__;
    ArkRadio radio;
    radio.type = ART_KEY_TuningUp;
    radio.length = 0;
    radio.data = NULL;
    ArkProtocol protocol;
    protocol.type = AT_Radio;
    protocol.direction = AD_Setter;
    protocol.data = static_cast<void*>(&radio);
    m_Private->onArkProtocol(protocol);

}

void RadioService::onArkProtocol(const ArkProtocolWrapper &protocol)
{
    qWarning() << __PRETTY_FUNCTION__ << protocol.type << protocol.data;
    if ((AT_Radio == protocol.type)
            && (NULL != protocol.data)) {
        struct ArkRadio* radio = arkProtocolWrapperToArkRadio(protocol);
        if (NULL != radio) {
            qWarning() << __PRETTY_FUNCTION__ << radio->type << radio->length << radio->data;
            switch (radio->type) {
            case ART_GetMinFreq:
                if (NULL != radio->data) {
                    printf("receive radio current min frequence is %d.\n", (radio->data[0] << 8) | radio->data[1]);
                }
                break;
            case ART_GetMaxFreq:
                if (NULL != radio->data) {
                    printf("receive radio current max frequence is %d.\n", (radio->data[0] << 8) | radio->data[1]);
                }
                break;
            case ART_GetGranularity:
                if (NULL != radio->data) {
                    printf("receive radio current granularity is %d.\n", radio->data[0]);
                }
                break;
            case ART_GetSignalIntensity:
                if (NULL != radio->data) {
                    printf("receive radio current sigal intensity is %d.\n", radio->data[0]);
                }
                break;
            case ART_GetStatus:
                if (NULL != radio->data) {
                    int status = *radio->data;
                    printf("radio current status is %d.\n", status);
                }
                break;
            case ART_GetCurfreqMono:
                printf("radio current frequence is mono.\n");
                break;
            case ART_GetCurfreqStereo:
                printf("radio current frequence is stereo.\n");
                break;
            case ART_GetUseMono:
                //                printf("radio use mono.\n");
                emit onStereoChange(RST_Mono);
                m_Private->m_RadioStereoType = RST_Mono;
                RadioPersistent::setStereo(m_Private->m_RadioStereoType);
                break;
            case ART_GetUseStereo:
                //                printf("radio use stereo.\n");
                emit onStereoChange(RST_Stereo);
                m_Private->m_RadioStereoType = RST_Stereo;
                RadioPersistent::setStereo(m_Private->m_RadioStereoType);
                break;
            case ART_GetAutoScan:
                printf("radio scan mode is auto.\n");
                break;
            case ART_GetManualScan:
                printf("radio scan mode is manual.\n");
                break;
            case ART_GetLOC:
                //                printf("radio is LOC now.\n");
                emit onLocChange(RLT_LocalSensitivityOn);
                m_Private->m_RadioLocType = RLT_LocalSensitivityOn;
                RadioPersistent::setLoc(m_Private->m_RadioLocType);
                break;
            case ART_GetDX:
                //                printf("radio is DX now.\n");
                emit onLocChange(RLT_LocalSensitivityOff);
                m_Private->m_RadioLocType = RLT_LocalSensitivityOff;
                RadioPersistent::setLoc(m_Private->m_RadioLocType);
                break;
            case ART_GetBand:
                if (NULL != radio->data) {
                    switch (*radio->data) {
                    case 0x00: {
                        m_Private->m_RadioBandType = RBT_FM1;
                        break;
                    }
                    case 0x01: {
                        m_Private->m_RadioBandType = RBT_FM2;
                        break;
                    }
                    case 0x02: {
                        m_Private->m_RadioBandType = RBT_FM3;
                        break;
                    }
                    case 0x03: {
                        m_Private->m_RadioBandType = RBT_AM1;
                        break;
                    }
                    case 0x04: {
                        m_Private->m_RadioBandType = RBT_AM2;
                        break;
                    }
                    default: {
                        break;
                    }
                    }
                    qWarning() << __PRETTY_FUNCTION__ << __LINE__ << "m_Private->m_RadioBandType" << m_Private->m_RadioBandType;
                    //                    printf("radio current band is %d.\n", *radio->data);
                }
                break;
            case ART_GetPreset:
                if (NULL != radio->data) {
                    m_Private->m_Index = short(*radio->data);
                    printf("radio preset channel is %d.\n", *radio->data);
                }
                break;
            case ART_GetFreq:
                if (NULL != radio->data) {
                    if (NULL != radio->data) {
                        m_Private->m_Frequency = (radio->data[0] << 8) | radio->data[1];
                        emit onRadioInfoChange(m_Private->m_RadioBandType, m_Private->m_Frequency, m_Private->m_Index);
                        //                        printf("radio preset frequence is %d.\n", (radio->data[0] << 8) | radio->data[1]);
                        //                        printf("radio preset frequence0 is %d.\n", radio->data[0]);
                        //                        printf("radio preset frequence1 is %d.\n", radio->data[1]);
                    }
                    //                    if (4 == radio->length) {
                    //                        switch (radio->data[0]) {
                    //                        case 0x00://FM
                    //                        case 0x01:
                    //                        case 0x02: {
                    //                            qWarning() << "m_Private->m_Time" << m_Private->m_Time.elapsed();
                    //                            unsigned short frequency = (radio->data[1] << 8) | radio->data[2];
                    ////                            qWarning() << "frequency" << frequency;
                    //                            emit onRadioInfoChange(m_Private->m_RadioBandType, frequency, m_Private->getIndexByBand(m_Private->m_RadioBandType, frequency));
                    //                            m_Private->m_BandFrequency.insert(m_Private->m_RadioBandType, frequency);
                    //                            RadioPersistent::setFrequency(m_Private->m_RadioBandType, frequency);
                    //                            break;
                    //                        }
                    //                        case 0x03://AM
                    //                        case 0x04: {
                    //                            unsigned short frequency = (radio->data[1] << 8) | radio->data[2];
                    //                            emit onRadioInfoChange(RBT_AM1, frequency, 0);
                    //                            break;
                    //                        }
                    //                        default: {
                    //                            break;
                    //                        }
                    //                        }
                    //                    }
                }
                break;
            case ART_GetPresetList:
                qWarning() << __PRETTY_FUNCTION__ << __LINE__;
                if (NULL != radio->data) {
                    int i;
                    for (i = 0; i < RADIO_PRESET_NUM; i++) {
                        printf("radio preset ch[%d] frequence is %d.\n", i, (radio->data[2*i] << 8) | radio->data[2*i + 1]);
                    }
                }
                break;
            }
        }
    }
}

//void RadioService::onTimeout()
//{
//}

RadioServicePrivate::RadioServicePrivate(RadioService *parent)
    : m_Parent(parent)
{
    m_RadioStereoType = RadioPersistent::getStereo();
    m_RadioLocType = RadioPersistent::getLoc();
    m_RadioBandType = RadioPersistent::getBand();
    m_Index = 0;
    m_Frequency = 0;
    m_Object = NULL;
    m_RadioServiceStatus = RSS_Undefine;
    initialize();
}

RadioServicePrivate::~RadioServicePrivate()
{
}

void RadioServicePrivate::initialize()
{
    m_BandFrequency.insert(RBT_FM1, RadioPersistent::getFrequency(RBT_FM1));
    //    m_BandFrequency.insert(RBT_FM2, RadioPersistent::getFrequency(RBT_FM2));
    //    m_BandFrequency.insert(RBT_FM3, RadioPersistent::getFrequency(RBT_FM3));
    m_BandFrequency.insert(RBT_AM1, RadioPersistent::getFrequency(RBT_AM1));
    //    m_BandFrequency.insert(RBT_AM2, RadioPersistent::getFrequency(RBT_AM2));

    m_BandStep.insert(RBT_FM1, RadioPersistent::getStep(RBT_FM1));
    //    m_BandStep.insert(RBT_FM2, RadioPersistent::getStep(RBT_FM2));
    //    m_BandStep.insert(RBT_FM3, RadioPersistent::getStep(RBT_FM3));
    m_BandStep.insert(RBT_AM1, RadioPersistent::getStep(RBT_AM1));
    //    m_BandStep.insert(RBT_AM2, RadioPersistent::getStep(RBT_AM2));

    m_MinFrequency.insert(RBT_FM1, RadioPersistent::getMinFrequency(RBT_FM1));
    //    m_MinFrequency.insert(RBT_FM2, RadioPersistent::getMinFrequency(RBT_FM2));
    //    m_MinFrequency.insert(RBT_FM3, RadioPersistent::getMinFrequency(RBT_FM3));
    m_MinFrequency.insert(RBT_AM1, RadioPersistent::getMinFrequency(RBT_AM1));
    //    m_MinFrequency.insert(RBT_AM2, RadioPersistent::getMinFrequency(RBT_AM2));

    m_MaxFrequency.insert(RBT_FM1, RadioPersistent::getMaxFrequency(RBT_FM1));
    //    m_MaxFrequency.insert(RBT_FM2, RadioPersistent::getMaxFrequency(RBT_FM2));
    //    m_MaxFrequency.insert(RBT_FM3, RadioPersistent::getMaxFrequency(RBT_FM3));
    m_MaxFrequency.insert(RBT_AM1, RadioPersistent::getMaxFrequency(RBT_AM1));
    //    m_MaxFrequency.insert(RBT_AM2, RadioPersistent::getMaxFrequency(RBT_AM2));

    m_BandList[RBT_FM1] = RadioPersistent::getFrequencyList(RBT_FM1);
    //    m_FM2FrequencyList = RadioPersistent::getFrequencyList(RBT_FM2);
    //    m_FM3FrequencyList = RadioPersistent::getFrequencyList(RBT_FM3);
    m_BandList[RBT_AM1] = RadioPersistent::getFrequencyList(RBT_AM1);
    m_Object = qApp->findChild<QObject*>(QString("Object"));
    assert(NULL != m_Object);
    QObject::connect(m_Object, SIGNAL(onArkProtocol(const ArkProtocolWrapper&)),
                     m_Parent, SLOT(onArkProtocol(const ArkProtocolWrapper&)));
}

void RadioServicePrivate::initializeSearchTimer()
{
    //    if (NULL == m_SearchTimer) {
    //        m_SearchTimer = new QTimer(m_Parent);
    //        m_SearchTimer->setSingleShot(false);
    //        m_SearchTimer->setInterval(33);
    //        QObject::connect(m_SearchTimer, SIGNAL(timeout()),
    //                         m_Parent,      SLOT(onTimeout()));
    //    }
}

void RadioServicePrivate::onArkProtocol(const ArkProtocol &protocol)
{
}

void RadioServicePrivate::setFrequency(const RadioBandType type, const unsigned short frequency)
{
    switch (type) {
    case RBT_FM1:
        /*    case RBT_FM2:
    case RBT_FM3:*/ {
        ArkRadio radio;
        radio.type = ART_KEY_FMFreq;
        unsigned char data[2] = {0};
        data[0] = frequency >> 8;
        data[1] = frequency & 0xFF;
        radio.length = 2;
        radio.data = data;
        ArkProtocol protocol;
        protocol.type = AT_Radio;
        protocol.direction = AD_Setter;
        protocol.data = static_cast<void*>(&radio);
        onArkProtocol(protocol);
        break;
    }
    case RBT_AM1:
        /*    case RBT_AM2:*/ {
        ArkRadio radio;
        radio.type = ART_KEY_AMFreq;
        unsigned char data[2] = {0};
        data[0] = frequency >> 8;
        data[1] = frequency & 0xFF;
        radio.length = 2;
        radio.data = data;
        ArkProtocol protocol;
        protocol.type = AT_Radio;
        protocol.direction = AD_Setter;
        protocol.data = static_cast<void*>(&radio);
        onArkProtocol(protocol);
        break;
    }
    default: {
        break;
    }
    }
}

short RadioServicePrivate::getIndexByBand(const RadioBandType type, const unsigned short frequency)
{
    short index(-1);
    if ((m_BandList.contains(type))
            && (m_BandList.value(type).contains(frequency))) {
        index = m_BandList.value(type).indexOf(frequency);
    }
    return index;
    //    switch (type) {
    //    case RBT_FM1: {
    //        if (m_FM1FrequencyList.contains(frequency)) {
    //            index = ;
    //        }
    //        break;
    //    }
    ////    case RBT_FM2: {
    ////        break;
    ////    }
    ////    case RBT_FM3: {
    ////        break;
    ////    }
    //    case RBT_AM1: {
    //        break;
    //    }
    ////    case RBT_AM2: {
    ////        break;
    ////    }
    //    default: {
    //        break;
    //    }
    //    }
}
