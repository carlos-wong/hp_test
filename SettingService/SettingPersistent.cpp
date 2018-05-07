#include "SettingPersistent.h"
#include "SettingService.h"
#include "ark_api.h"
#include "UserInterfaceUtility.h"
#include <QSettings>
#include <QCoreApplication>
#include <QFile>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

#define BACKLIGHT_FILE "/sys/devices/platform/pwm-backlight/backlight/pwm-backlight/brightness"
static int bri_fd = -1;
static unsigned char g_brightness;
static bool illbri_is_set = false;

static const QString persistentPath("/data/Setting.ini");
static const QString persistentPathBackup("/data/SettingBackup.ini");
static const QString Exists("Exists");
static const QString Calibrate("Calibrate");
static const QString Language("Language");
static const QString Brightness("Brightness");
static const QString Contrast("Contrast");
static const QString Saturation("Saturation");
static const QString Track("Track");
static const QString Radar("Radar");
static const QString Camera("Camera");
static const QString AssistTrack("AssistTrack");
static const QString McuVersion("McuVersion");
static const QString HourClock("HourClockType");
static const QString Theme("Theme");
static const QString CarLink("CarLink");
static const QString Logo("Logo");
static const QString BrakeStatus("BrakeStatus");
static const QString ReverseMute("ReverseMute");

static const unsigned char brightnessStep(3);
static const unsigned char maxbrightness(43);
static const unsigned char minbrightness(0);

static const unsigned char constrasStep(12);
static const unsigned char minimumConstrass(59);

static const unsigned char saturationStep(10);
static const unsigned char minimumSaturation(101);

class SettingSettings : public QSettings
{
    Q_DISABLE_COPY(SettingSettings)
#ifdef g_Settings
#undef g_Settings
#endif
#define g_Settings (SettingSettings::instance())
public:
    inline static SettingSettings* instance() {
        static SettingSettings* settings(new SettingSettings(qApp));
        return settings;
    }
    void clear();
    void sync();
    void setValue(const QString &key, const QVariant &value);
private:
    explicit SettingSettings(QObject* parent = NULL);
    ~SettingSettings();
    QSettings m_SettingsBackup;
};

void SettingPersistent::setLanguage(const int value)
{
    g_Settings->setValue(Language, value);
    g_Settings->sync();
}

void SettingPersistent::setCurrentBrakeStatus(const bool value)
{
    g_Settings->setValue(BrakeStatus, value);
    g_Settings->sync();
}

bool SettingPersistent::getCurrentBrakeStatus(const bool reload)
{
    bool flag(true);
    if (reload) {
        g_Settings->sync();
    }
    bool ret = g_Settings->value(BrakeStatus, flag).toBool();
    return ret;
}

void SettingPersistent::setReverseMute(const bool value)
{
    g_Settings->setValue(ReverseMute, value);
    g_Settings->sync();
}

bool SettingPersistent::getReverseMute(const bool reload)
{
    bool flag(true);
    if (reload) {
        g_Settings->sync();
    }
    bool ret = g_Settings->value(ReverseMute, flag).toBool();
    return ret;
}


bool SettingPersistent::restoreBrightness()
{
    // int ret = arkapi_set_layer_brightness(OSD1_LAYER, 45 + 14 * getBrightness());
    // return (0 == ret);
    int ret = arkapi_set_layer_brightness(OSD1_LAYER, 100);//Ĭlayer brightness
    
    int brightness = maxbrightness - getBrightness()*brightnessStep;

    bri_fd = open(BACKLIGHT_FILE, O_RDWR);
    if (bri_fd < 0)
    {
        printf("failed to open :%s\n", BACKLIGHT_FILE);
        return false;
    }else{
        printf("restoreBrightness: = %d\n", brightness);
        char buffer[20];
        int bytes = sprintf(buffer, "%d\n", brightness);
        write(bri_fd, buffer, bytes);
    }

    return true;
}

bool SettingPersistent::restoreContrast()
{
    // int ret = arkapi_set_layer_contrast(OSD1_LAYER, 45 + 14 * getContrast());
    // printf("%s %d\n", __func__, minimumConstrass + constrasStep * getContrast());
    int ret = arkapi_set_layer_contrast(OSD1_LAYER, minimumConstrass + constrasStep * getContrast());
    return (0 == ret);
}

bool SettingPersistent::restoreSaturation()
{
    // int ret = arkapi_set_layer_saturation(OSD1_LAYER, 45 + 14 * getSaturation());
    // printf("%s %d\n", __func__, minimumSaturation + saturationStep * getSaturation());
    int ret = arkapi_set_layer_saturation(OSD1_LAYER, minimumSaturation + saturationStep * getSaturation());

    return (0 == ret);
}

bool SettingPersistent::illDetectSetBrightness(bool on)
{
    int brightness = getBrightness();
    if (on)
    {
        if (bri_fd > 0 && brightness >= 3)
        {
            illbri_is_set = true;
            g_brightness = brightness;
            brightness = brightness - 4;
            if (brightness < 3)
            {
                brightness = 3;
            }
            brightness = maxbrightness - brightness * brightnessStep;
            printf("%s: set: = %d\n", __func__, brightness);
            char buffer[20];
            int bytes = sprintf(buffer, "%d\n", brightness);
            write(bri_fd, buffer, bytes);

        }
    }else if (illbri_is_set == true)
    {
        illbri_is_set = false;
        brightness = maxbrightness - g_brightness * brightnessStep;
        printf("%s: restore: = %d\n", __func__, brightness);
        char buffer[20];
        int bytes = sprintf(buffer, "%d\n", brightness);
        write(bri_fd, buffer, bytes);
    }
    return true;
}


int SettingPersistent::getLanguage(const bool update)
{
    int defaultValue(LT_Chinese);
    bool flag(false);
    if (update) {
        g_Settings->sync();
    }
    int ret = g_Settings->value(Language, defaultValue).toInt(&flag);
    if (!flag) {
        ret = defaultValue;
    }
    return ret;
}

void SettingPersistent::setBrightness(const int value)
{
    g_Settings->setValue(Brightness, value);
    g_Settings->sync();

    int brightness = maxbrightness - value; 
    if (bri_fd > 0)
    {
        char buffer[20];
        int bytes = sprintf(buffer, "%d\n", brightness);
        printf("setBrightness: = %d\n", brightness);
        write(bri_fd, buffer, bytes);
    }
}

int SettingPersistent::getBrightness(const bool reload)
{
    int defaultValue(30);
    bool flag(false);
    if (reload) {
        g_Settings->sync();
    }
    if (!g_Settings->contains(Brightness)) {
        defaultValue = 30;
    }
    int ret = g_Settings->value(Brightness, defaultValue).toInt(&flag);
    if (!flag) {
        ret = defaultValue;
    }
    ret = ret/brightnessStep;
    return ret;
}

void SettingPersistent::setContrast(const int value)
{
    g_Settings->setValue(Contrast, value);
    g_Settings->sync();
}

int SettingPersistent::getContrast(const bool reload)
{
    int defaultValue(7);
    bool flag(false);
    if (reload) {
        g_Settings->sync();
    }
    // if (!g_Settings->contains(Contrast)) {
    //     defaultValue = (arkapi_get_layer_contrast(OSD1_LAYER) - 45) / 14;
    // }
    int ret = g_Settings->value(Contrast, defaultValue).toInt(&flag);
    if (!flag) {
        ret = defaultValue;
    }
    return ret;
}

void SettingPersistent::setSaturation(const int value)
{
    g_Settings->setValue(Saturation, value);
    g_Settings->sync();
}

void SettingPersistent::setReversingTrack(const int value)
{
    g_Settings->setValue(Track, value);
    g_Settings->sync();
}

void SettingPersistent::setReversingRadar(const int value)
{
    g_Settings->setValue(Radar, value);
    g_Settings->sync();
}

void SettingPersistent::setReversingAssistTrack(const int value)
{
    g_Settings->setValue(AssistTrack, value);
    g_Settings->sync();
}

void SettingPersistent::setCamara(const int value)
{
    g_Settings->setValue(Camera, value);
    g_Settings->sync();
}

void SettingPersistent::setCalibrate(const bool flag)
{
    g_Settings->setValue(Calibrate, flag);
    g_Settings->sync();
}

void SettingPersistent::setMcuVersion(const QString& version)
{
    g_Settings->setValue(McuVersion, version);
    g_Settings->sync();
}

void SettingPersistent::setHourClockType(const int type)
{
    g_Settings->setValue(HourClock, type);
    g_Settings->sync();
}

void SettingPersistent::setThemeType(const int type)
{
    g_Settings->setValue(Theme, type);
    g_Settings->sync();
}

void SettingPersistent::setCarLinkType(const int type)
{
    g_Settings->setValue(CarLink, type);
    g_Settings->sync();
}

void SettingPersistent::setLogoType(const int type)
{
    g_Settings->setValue(Logo, type);
    g_Settings->sync();
}

void SettingPersistent::reset()
{
    g_Settings->clear();
    g_Settings->sync();
}

int SettingPersistent::getSaturation(const bool reload)
{
    int defaultValue(7);
    bool flag(false);
    if (reload) {
        g_Settings->sync();
    }
    // if (!g_Settings->contains(Saturation)) {
    //     defaultValue = (arkapi_get_layer_saturation(OSD1_LAYER) - 45) / 14;
    // }
    int ret = g_Settings->value(Saturation, defaultValue).toInt(&flag);
    if (!flag) {
        ret = defaultValue;
    }
    return ret;
}

int SettingPersistent::getReversingTrack(const bool reload)
{
    int defaultValue(1);
    bool flag(false);
    if (reload) {
        g_Settings->sync();
    }
    int ret = g_Settings->value(Track, defaultValue).toInt(&flag);
    if (!flag) {
        ret = defaultValue;
    }
    return ret;
}

int SettingPersistent::getReversingRadar(const bool reload)
{
    int defaultValue(1);
    bool flag(false);
    if (reload) {
        g_Settings->sync();
    }
    int ret = g_Settings->value(Radar, defaultValue).toInt(&flag);
    if (!flag) {
        ret = defaultValue;
    }
    return ret;
}

int SettingPersistent::getReversingAssistTrack(const bool reload)
{
    int defaultValue(1);
    bool flag(false);
    if (reload) {
        g_Settings->sync();
    }
    int ret = g_Settings->value(AssistTrack, defaultValue).toInt(&flag);
    if (!flag) {
        ret = defaultValue;
    }
    return ret;
}

bool SettingPersistent::getCalibrate(const bool reload)
{
    bool flag(false);
    if (reload) {
        g_Settings->sync();
    }
    bool ret = g_Settings->value(Calibrate, flag).toBool();
    return ret;
}

QString SettingPersistent::getMcuVersion(const bool reload)
{
    QString flag("xxxxxxxxMCUx.x");
    if (reload) {
        g_Settings->sync();
    }
    QString ret = g_Settings->value(McuVersion, flag).toString();
    return ret;
}

int SettingPersistent::getHourClockType(const bool reload)
{
    int defaultValue(HCT_12HourClock);
    bool flag(false);
    if (reload) {
        g_Settings->sync();
    }
    int ret = g_Settings->value(HourClock, defaultValue).toInt(&flag);
    if (!flag) {
        ret = defaultValue;
    }
    return ret;
}

int SettingPersistent::getThemeType(const bool reload)
{
    int defaultValue(TT_ThemeOne);
    bool flag(false);
    if (reload) {
        g_Settings->sync();
    }
    int ret = g_Settings->value(Theme, defaultValue).toInt(&flag);
    if (!flag) {
        ret = defaultValue;
    }
    return ret;
}

int SettingPersistent::getCarLinkType(const bool reload)
{
    int defaultValue(CLT_CarlifeCarplay);
    bool flag(false);
    if (reload) {
        g_Settings->sync();
    }
    int ret = g_Settings->value(CarLink, defaultValue).toInt(&flag);
    if (!flag) {
        ret = defaultValue;
    }
    return ret;
}

int SettingPersistent::getLogoType(const bool reload)
{
    int defaultValue(1);
    bool flag(false);
    if (reload) {
        g_Settings->sync();
    }
    int ret = g_Settings->value(Logo, defaultValue).toInt(&flag);
    if (!flag) {
        ret = defaultValue;
    }
    return ret;
}

int SettingPersistent::getCamera(const bool reload)
{
    int defaultValue(1);
    bool flag(false);
    if (reload) {
        g_Settings->sync();
    }
    int ret = g_Settings->value(Camera, defaultValue).toInt(&flag);
    if (!flag) {
        ret = defaultValue;
    }
    return ret;
}

void SettingSettings::clear()
{
    QSettings::clear();
    m_SettingsBackup.clear();
}

void SettingSettings::sync()
{
    QSettings::sync();
    m_SettingsBackup.sync();
}

void SettingSettings::setValue(const QString &key, const QVariant &value)
{
    if (!contains(Exists)) {
        QSettings::setValue(Exists, QVariant(bool(true)));
    }
    QSettings::setValue(key, value);
    if (!m_SettingsBackup.contains(Exists)) {
        m_SettingsBackup.setValue(Exists, QVariant(bool(true)));
    }
    m_SettingsBackup.setValue(key, value);
}

SettingSettings::SettingSettings(QObject *parent)
    : QSettings(persistentPath, QSettings::IniFormat, parent)
    , m_SettingsBackup(persistentPathBackup, QSettings::IniFormat, parent)
{
    bool ret(true);
    if (contains(Exists) && (!m_SettingsBackup.contains(Exists))) {
        QFile::remove(persistentPathBackup);
        ret = QFile::copy(persistentPath, persistentPathBackup);
    } else if ((!contains(Exists)) && m_SettingsBackup.contains(Exists)) {
        QFile::remove(persistentPath);
        ret = QFile::copy(persistentPathBackup, persistentPath);
    }
    if (!ret) {
        fprintf(stderr, "copy failed!");
    }
}

SettingSettings::~SettingSettings()
{
}
