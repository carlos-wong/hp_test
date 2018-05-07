#include "BluetoothPersistent.h"
#include <QSettings>
#include <stdlib.h>
#include <QApplication>
#include <QFile>
#include <QDebug>

static const QString persistentPath("/data/Bluetooth.ini");
static const QString persistentPathBackup("/data/BluetoothBackup.ini");
static const QString RemoteAddress("RemoteAddress");
static const QString Exists("Exists");

class SettingSettings : public QSettings
{
    Q_DISABLE_COPY(SettingSettings)
#ifdef g_SettingSettings
#undef g_SettingSettings
#endif
#define g_SettingSettings (SettingSettings::instance())
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

void BluetoothPersistent::setRemoteAddress(const QString &address)
{
    g_SettingSettings->setValue(RemoteAddress, address);
    g_SettingSettings->sync();
}

QString BluetoothPersistent::getRemoteAddress(const bool reload)
{
    QString flag("");
    if (reload) {
        g_SettingSettings->sync();
    }
    QString ret = g_SettingSettings->value(RemoteAddress, flag).toString();
    return ret;
}

BluetoothPersistent::BluetoothPersistent()
{
}

BluetoothPersistent::~BluetoothPersistent()
{
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
        qWarning() << "copy failed!";
    }
}
