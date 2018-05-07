#ifndef SETTING_H
#define SETTING_H

#include "ArkProtocolWrapper.h"
#include "SettingService.h"
#include <QObject>
#include <QScopedPointer>
#include <QCoreApplication>
#include <QMap>

class SettingPrivate;
class Setting
        : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(Setting)
#ifdef g_Setting
#undef g_Setting
#endif
#define g_Setting (Setting::instance())
public:
    inline static Setting* instance() {
        static Setting* setting(new Setting(qApp));
        return setting;
    }
    void reset();
    void initializeArkProtocol();
    void setStartComplete();
    bool startComplete();
    void setLanguage(const int language);
    void setBrightness(const int type, const int value);
    void setContrast(const int type, const int value);
    void setSaturation(const int type, const int value);
    void setDisplayScreen(const int type);
    void startAV(const int type, const int width, const int height);
    void stopAV(const int type);
    void toggleHourClock();
    void setThemeType(const int type);
    void synchronizeSteeringWheelStatus();
    void steeringStudy(const StudySteeringWheelType type, const StudySteeringWheelStatus status);
    const QMap<StudySteeringWheelStatus, struct StudySteeringWheel>& getSteeringStudy();
    ArkCarStatus getReversing();
    DisplayScreenType getDisplayScreenType();
    void setArkProtocol(const ArkProtocolWrapper& protocol);
    bool startMCUUpgrade();
    void requestMCUVersion();
    bool setLauncherTVOUT(const bool flag);
signals:
    void onStartComplete();
    void onLanguageTranslateChange(const int language);
    void onBrightnessChange(const int value);
    void onContrastChange(const int value);
    void onSaturationChange(const int value);
    void onDisplayScreenChange(const int type);
    void onAVStatus(const int type, const int status);
    void onHourClockChange(const int type);
    void onThemeChange(const int type);
    void onSteeringStudyChange(const QMap<StudySteeringWheelStatus, struct StudySteeringWheel>& map);
    void onArkProtocol(const ArkProtocolWrapper& protocol);
    void onBrakestatusChange(const int status);
private slots:
    void onLanguageChange(const int language);
    void onDisplayScreenHandler(const int type);
    void onArkProtocolHandler(const ArkProtocolWrapper& protocol);
protected:
    void timerEvent(QTimerEvent* event);
private slots:
    void onTimeout();
private:
    explicit Setting(QObject *parent = NULL);
    ~Setting();
    void initializePrivate();
    friend class SettingPrivate;
    QScopedPointer<SettingPrivate> m_Private;
};

#endif // SETTING_H
