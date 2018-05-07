#include "AudioPersistent.h"
#include "ark_api.h"
#include <QDebug>
#include <QFile>
#include <QSettings>
#include <QCoreApplication>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#ifdef gcc
static const QString persistentPath("/tmp/Audio.ini");
static const QString persistentPathBackup("/tmp/AudioBackup.ini");
#else
static const QString persistentPath("/data/Audio.ini");
static const QString persistentPathBackup("/data/AudioBackup.ini");
#endif
static const QString Exists("Exists");
static const QString Volume("Volume");
static const QString Equalizer("Equalizer");
static const QString Bass("Bass");
static const QString Middle("Middle");
static const QString Treble("Treble");
static const QString Sound("Sound");
static const QString Left("Left");
static const QString Right("Right");

class AudioSettings : public QSettings
{
    Q_DISABLE_COPY(AudioSettings)
#define g_Settings (AudioSettings::instance())
public:
    inline static AudioSettings* instance() {
        static AudioSettings* settings(new AudioSettings(qApp));
        return settings;
    }
    void clear();
    void sync();
    void setValue(const QString &key, const QVariant &value);
private:
    explicit AudioSettings(QObject* parent = NULL);
    ~AudioSettings();
    QSettings m_SettingsBackup;
};

void AudioPersistent::reset()
{
    g_Settings->clear();
    g_Settings->sync();
}

int AudioPersistent::getVolume(const bool reload)
{
    int defaultValue(40);
    if (QString(qgetenv("PROTOCOL_ID")).contains(QString("ark169"))) {
        defaultValue = 25;
    } else if (QString(qgetenv("PROTOCOL_ID")).contains(QString("yaoxi"))) {
        defaultValue = 25;
    }
    bool flag(false);
    if (reload) {
        g_Settings->sync();
    }
    int ret = g_Settings->value(Volume, defaultValue).toInt(&flag);
    if (!flag) {
        ret = defaultValue;
    }
    return ret;
}

int AudioPersistent::getEqualizerItem(const bool reload)
{
    int defaultValue(EI_Standard);
    bool flag(false);
    if (reload) {
        g_Settings->sync();
    }
    int ret = g_Settings->value(Equalizer, defaultValue).toInt(&flag);
    if (!flag)
    {
        ret = defaultValue;
    }
    return ret;
}

int AudioPersistent::getBassEqualizer(const bool reload)
{
    int defaultValue(7);
    bool flag(false);
    if (reload) {
        g_Settings->sync();
    }
    int ret = g_Settings->value(Bass, defaultValue).toInt(&flag);
    if (!flag)
    {
        ret = defaultValue;
    }
    return ret;
}

int AudioPersistent::getMiddleEqualizer(const bool reload)
{
    int defaultValue(7);
    bool flag(false);
    if (reload) {
        g_Settings->sync();
    }
    int ret = g_Settings->value(Middle, defaultValue).toInt(&flag);
    if (!flag)
    {
        ret = defaultValue;
    }
    return ret;
}

int AudioPersistent::getTrebleEqualizer(const bool reload)
{
    int defaultValue(7);
    bool flag(false);
    if (reload) {
        g_Settings->sync();
    }
    int ret = g_Settings->value(Treble, defaultValue).toInt(&flag);
    if (!flag)
    {
        ret = defaultValue;
    }
    return ret;
}

void AudioPersistent::setEqualizerItem(const int item)
{
    g_Settings->setValue(Equalizer, item);
    g_Settings->sync();
}

void AudioPersistent::setBassEqualizer(const int value)
{
    g_Settings->setValue(Bass, value);
    g_Settings->sync();
}

void AudioPersistent::setMiddleEqualizer(const int value)
{
    g_Settings->setValue(Middle, value);
    g_Settings->sync();
}

void AudioPersistent::setTrebleEqualizer(const int value)
{
    g_Settings->setValue(Treble, value);
    g_Settings->sync();
}

int AudioPersistent::getSoundItem(const bool reload)
{
    int defaultValue(SI_Custom);
    bool flag(false);
    if (reload) {
        g_Settings->sync();
    }
    int ret = g_Settings->value(Sound, defaultValue).toInt(&flag);
    if (!flag)
    {
        ret = defaultValue;
    }
    return ret;
}

int AudioPersistent::getLeftSound(const bool reload)
{
    int defaultValue(0);
    bool flag(false);
    if (reload) {
        g_Settings->sync();
    }
    int ret = g_Settings->value(Left, defaultValue).toInt(&flag);
    if (!flag)
    {
        ret = defaultValue;
    }
    return ret;
}

int AudioPersistent::getRightSound(const bool reload)
{
    int defaultValue(0);
    bool flag(false);
    if (reload) {
        g_Settings->sync();
    }
    int ret = g_Settings->value(Right, defaultValue).toInt(&flag);
    if (!flag)
    {
        ret = defaultValue;
    }
    return ret;
}

void AudioPersistent::setSoundItem(const int item)
{
    g_Settings->setValue(Sound, item);
    g_Settings->sync();
}

void AudioPersistent::setLeftSound(const int value)
{
    g_Settings->setValue(Left, value);
    g_Settings->sync();
}

void AudioPersistent::setRightSound(const int value)
{
    g_Settings->setValue(Right, value);
    g_Settings->sync();
}

void AudioPersistent::setVolume(int value)
{
    g_Settings->setValue(Volume, value);
    g_Settings->sync();
}

void AudioSettings::clear()
{
    QSettings::clear();
    m_SettingsBackup.clear();
}

void AudioSettings::sync()
{
    QSettings::sync();
    m_SettingsBackup.sync();
}

void AudioSettings::setValue(const QString &key, const QVariant &value)
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

AudioSettings::AudioSettings(QObject *parent)
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
        qDebug() << "copy failed!";
    }
}

AudioSettings::~AudioSettings()
{
}
