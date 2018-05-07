#ifndef SETTINGSERVICE_H
#define SETTINGSERVICE_H

#include "SettingPersistent.h"
#include <QObject>
#include <QScopedPointer>

class SettingServicePrivate;
class SettingService : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(SettingService)
public:
    explicit SettingService(QObject* parent = NULL);
    ~SettingService();
public slots:
    void synchronize();
    void reset();
    void setLanguage(const int language);
    void setBrightness(const int type, const int value);
    void setContrast(const int type, const int value);
    void setSaturation(const int type, const int value);
    void setDisplayScreen(const int type);
    void toggleHourClock();
    void startAV(const int type, const int width, const int height);
    void stopAV(const int type);
    void setThemeType(const int type);
signals:
    void onLanguageChange(const int language);
    void onBrightnessChange(const int value);
    void onContrastChange(const int value);
    void onSaturationChange(const int value);
    void onDisplayScreenChange(const int type);
    void onAVStatus(const int type, const int status);
    void onHourClockChange(const int type);
    void onThemeChange(const int type);
private slots:
    void onTimeout();
private:
    friend class SettingServicePrivate;
    QScopedPointer<SettingServicePrivate> m_Private;
};

#endif // SETTINGSERVICE_H
