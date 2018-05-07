#ifndef SETTINGPERSISTENT_H
#define SETTINGPERSISTENT_H

#include <QtGlobal>

enum StudySteeringWheelType {
    SSWT_Undefine = -1,
    SSWT_StartSample,
    SSWT_ExitSample,
    SSWT_SaveSample,
    SSWT_ResetSample,
    SSWT_StudyCode,
};

enum StudySteeringWheelStatus {
    SSWS_Undefine = -1,
    SSWS_Ok,
    SSWS_Cancel,
    SSWS_Previous,
    SSWS_Next,
    SSWS_Power,
    SSWS_Mute,
    SSWS_VolumeAdd,
    SSWS_VolumeSub,
    SSWS_Answer,
    SSWS_Mode,
    SSWS_Handup,
};

struct StudySteeringWheel {
    StudySteeringWheelStatus type;
    StudySteeringWheelStatus status;
};

enum LanguageType {
    LT_English = 0,
    LT_Chinese = 1,
    LT_Japaness = 2,
    LT_Korean = 3,
    LT_Spanish = 4,
    LT_Portuguese = 5,
    LT_Russian = 6,
    LT_German = 7,
    LT_French = 8,
};
#define LanguageType int

enum SettingType {
    ST_Value = 0,
    ST_Plus = 1,
    ST_Minus = 2,
};
#define SettingType int

enum AVStatus {
    AVS_NoDetact = 0,
    AVS_Detacted,
};
#define AVStatus int

enum DisplayScreenType {
    DST_On = 0,
    DST_Off = 1
};
#define DisplayScreenType int

enum AVType {
    AVT_Undefine = -1,
    AVT_0,
    AVT_1,
    AVT_2
};
#define AVType int

enum HourClockType {
    HCT_Undefine = -1,
    HCT_12HourClock,
    HCT_24HourClock,
};
#define HourClockType int

enum ThemeType {
    TT_Undefine = -1,
    TT_ThemeOne,
    TT_ThemeTwo,
    TT_ThemeThree,
    TT_ThemeFour,
    TT_ThemeFive,
    TT_ThemeSix,
};
#define ThemeType int

enum CarLinkType {
    CLT_Undefine = -1,
    CLT_CarlifeCarplay,
    CLT_AutoCarplay,
};
#define CarLinkType int

class SettingPersistent
{
    Q_DISABLE_COPY(SettingPersistent)
public:
    static bool restoreBrightness();
    static bool restoreContrast();
    static bool restoreSaturation();
    static int getLanguage(const bool reload = false);
    static int getBrightness(const bool reload = false);
    static int getContrast(const bool reload = false);
    static int getSaturation(const bool reload = false);
    static int getCamera(const bool reload = false);
    static int getReversingTrack(const bool reload = false);
    static int getReversingRadar(const bool reload = false);
    static int getReversingAssistTrack(const bool reload = false);
    static bool getCalibrate(const bool reload = false);
    static bool getCurrentBrakeStatus(const bool reload = false);
    static QString getMcuVersion(const bool reload = false);
    static int getHourClockType(const bool reload = false);
    static int getThemeType(const bool reload = false);
    static int getCarLinkType(const bool reload = false);
    static int getLogoType(const bool reload = false);
    static bool getReverseMute(const bool reload = false);
    static void setLanguage(const int value);
    static void setBrightness(const int value);
    static void setContrast(const int value);
    static void setSaturation(const int value);
    static void setReversingTrack(const int value);
    static void setReversingRadar(const int value);
    static void setReversingAssistTrack(const int value);
    static void setCamara(const int value);
    static void setCalibrate(const bool flag);
    static void setMcuVersion(const QString& version);
    static void setHourClockType(const int type);
    static void setThemeType(const int type);
    static void setCarLinkType(const int type);
    static void setLogoType(const int type);
    static bool illDetectSetBrightness(const bool on);
    static void setCurrentBrakeStatus(const bool status);
    static void setReverseMute(const bool value);
    static void reset();
};

#endif // SETTINGPERSISTENT_H
