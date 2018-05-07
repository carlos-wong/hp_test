#ifndef RADIOPERSISTENT_H
#define RADIOPERSISTENT_H

#include <QList>
#include <QVariant>
#include <QtGlobal>

enum RadioStereoType {
    RST_Undefine = -1,
    RST_Stereo,
    RST_Mono,
};

enum RadioLocType {
    RLT_Undefine = -1,
    RLT_LocalSensitivityOn,
    RLT_LocalSensitivityOff,
};

enum RadioWorkType {
    RWT_Undefine = -1,
    RWT_Idle,
    RWT_SeekUp,
    RWT_SeekDown,
    RWT_AutoStore,
    RWT_BrowseStore,
};

enum RadioBandType {
    RBT_Undefine = -1,
    RBT_FM1,
    RBT_FM2,
    RBT_FM3,
    RBT_AM1,
    RBT_AM2,
};

#if 0
//欧洲频率
IAR_CONST u16 FmFreqAreaEup[]={8750,8750,9010,9810,10610,10800,8750};
IAR_CONST u16 AmFreqAreaEup[]={522,522,603,999,1404,1620,522};
//美洲1 频率
IAR_CONST u16 FmFreqAreaUSA1[]={8750,8750,9010,9810,10610,10790,8750};
IAR_CONST u16 AmFreqAreaUSA1[]={530,530,600,1000,1400,1710,530};
//美洲2 频率
IAR_CONST u16 FmFreqAreaUSA2[]={8750,8750,9010,9810,10610,10800,8750};
IAR_CONST u16 AmFreqAreaUSA2[]={530,530,600,1000,1400,1720,530};
//俄罗斯频率
IAR_CONST u16  FmFreqAreaRuss[]={6500,6500,6710,7040,7250,7400,6500};
IAR_CONST u16  AmFreqAreaRuss[]={522,522,603,999,1404,1620,522};
//日本频率
IAR_CONST u16 FmFreqAreaJapan[]={7600,7600,7640,8560,8700,9000,7600};
IAR_CONST u16 AmFreqAreaJapan[]={522,522,603,999,1404,1629,522};
// 步进频率
IAR_CONST u8 RadioStepFreqEup[2]={10,9,};
IAR_CONST u8 RadioStepFreqUSA1[2]={20,10,};
IAR_CONST u8 RadioStepFreqUSA2[2]={10,10,};
IAR_CONST u8 RadioStepFreqRuss[2]={3,9,};
IAR_CONST u8 RadioStepFreqJapan[2]={1,9,};
#endif

class RadioPersistent
{
    Q_DISABLE_COPY(RadioPersistent)
public:
    static void reset();
    static unsigned short getMinFrequency(const RadioBandType type);
    static unsigned short getMaxFrequency(const RadioBandType type);
    static unsigned short getStep(const RadioBandType type);
    static void setBand(const RadioBandType type);
    static RadioBandType getBand(const bool reload = false);
    static void setFrequencyList(const RadioBandType type, const QList<QVariant>& list);
    static QList<QVariant> getFrequencyList(const RadioBandType type, const bool reload = false);
    static void setFrequencyListIndex(const RadioBandType type, const unsigned short index);
    static unsigned short getFrequencyListIndex(const RadioBandType type, const bool reload = false);
    static void setStereo(const RadioStereoType type);
    static void setFrequency(const RadioBandType type, const unsigned short frequency);
    static unsigned short getFrequency(const RadioBandType type, const bool reload = false);
    static RadioStereoType getStereo(const bool reload = false);
    static void setLoc(const RadioLocType type);
    static RadioLocType getLoc(const bool reload = false);
    RadioPersistent();
    ~RadioPersistent();
};

#endif // RADIOPERSISTENT_H
