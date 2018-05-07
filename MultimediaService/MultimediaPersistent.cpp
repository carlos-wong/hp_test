#include "MultimediaPersistent.h"
#include <QDebug>
#include <QFile>
#include <QSettings>
#include <QCoreApplication>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

static const QString persistentPath("/data/Multimedia.ini");
static const QString persistentPathBackup("/data/MultimediaBackup.ini");
static const QString Exists("Exists");
static const QString SDMultimedia("SDMultimedia");
static const QString USBMultimedia("USBMultimedia");
static const QString SDMusicMillesmial("SDMusicMillesmial");
static const QString SDMusicPathInfo("SDMusicPathInfo");
static const QString USBMusicMillesmial("USBMusicMillesmial");
static const QString USBMusicPathInfo("USBMusicPathInfo");
static const QString SDImagePathInfo("SDImagePathInfo");
static const QString USBImagePathInfo("USBImagePathInfo");
static const QString SDVideoMillesmial("SDVideoMillesmial");
static const QString SDVideoPathInfo("SDVideoPathInfo");
static const QString USBVideoMillesmial("USBVideoMillesmial");
static const QString USBVideoPathInfo("USBVideoPathInfo");

class MultimediaSettings : public QSettings
{
    Q_DISABLE_COPY(MultimediaSettings)
#ifdef g_Settings
#undef g_Settings
#endif
#define g_Settings (MultimediaSettings::instance())
public:
    inline static MultimediaSettings* instance() {
        static MultimediaSettings* settings(new MultimediaSettings(qApp));
        return settings;
    }
    void clear();
    void sync();
    void setValue(const QString &key, const QVariant &value);
private:
    explicit MultimediaSettings(QObject* parent = NULL);
    ~MultimediaSettings();
    QSettings m_SettingsBackup;
};

void MultimediaSettings::clear()
{
    QSettings::clear();
    m_SettingsBackup.clear();
}

void MultimediaSettings::sync()
{
    QSettings::sync();
    m_SettingsBackup.sync();
}

void MultimediaSettings::setValue(const QString &key, const QVariant &value)
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

MultimediaSettings::MultimediaSettings(QObject *parent)
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
        qWarning() << __FUNCTION__ << "copy failed!";
    }
}

MultimediaSettings::~MultimediaSettings()
{
    qDebug();
}

int MultimediaPersistent::getMusicMillesmial(const DeviceWatcherType type, const bool reload)
{
    QString pathInfo("");
    switch (type) {
    case DWT_SDDisk: {
        pathInfo = SDMusicMillesmial;
        break;
    }
    case DWT_USBDisk: {
        pathInfo = USBMusicMillesmial;
        break;
    }
    default: {
        return 0;
        break;
    }
    }
    if (reload) {
        g_Settings->sync();
    }
    bool flag(false);
    int ret = g_Settings->value(pathInfo, QVariant(0)).toInt(&flag);
    if (!flag) {
        ret = 0;
    }
    return ret;
}

QString MultimediaPersistent::getMusicPathInfo(const DeviceWatcherType type, const bool reload)
{
    QString pathInfo("");
    switch (type) {
    case DWT_SDDisk: {
        pathInfo = SDMusicPathInfo;
        break;
    }
    case DWT_USBDisk: {
        pathInfo = USBMusicPathInfo;
        break;
    }
    default: {
        return pathInfo;
        break;
    }
    }
    if (reload) {
        g_Settings->sync();
    }
    return g_Settings->value(pathInfo, QVariant(QString(""))).toString();
}

QString MultimediaPersistent::getImagePathInfo(const DeviceWatcherType type, const bool reload)
{
    QString pathInfo("");
    switch (type) {
    case DWT_SDDisk: {
        pathInfo = SDImagePathInfo;
        break;
    }
    case DWT_USBDisk: {
        pathInfo = USBImagePathInfo;
        break;
    }
    default: {
        return pathInfo;
        break;
    }
    }
    if (reload) {
        g_Settings->sync();
    }
    return g_Settings->value(pathInfo, QVariant(QString(""))).toString();
}

int MultimediaPersistent::getVideoMillesmial(const DeviceWatcherType type, const bool reload)
{
    QString pathInfo("");
    switch (type) {
    case DWT_SDDisk: {
        pathInfo = SDVideoMillesmial;
        break;
    }
    case DWT_USBDisk: {
        pathInfo = USBVideoMillesmial;
        break;
    }
    default: {
        return 0;
        break;
    }
    }
    if (reload) {
        g_Settings->sync();
    }
    bool flag(false);
    int ret = g_Settings->value(pathInfo, QVariant(0)).toInt(&flag);
    if (!flag) {
        ret = 0;
    }
    return ret;
}

QString MultimediaPersistent::getVideoPathInfo(const DeviceWatcherType type, const bool reload)
{
    QString pathInfo("");
    switch (type) {
    case DWT_SDDisk: {
        pathInfo = SDVideoPathInfo;
        break;
    }
    case DWT_USBDisk: {
        pathInfo = USBVideoPathInfo;
        break;
    }
    default: {
        return pathInfo;
        break;
    }
    }
    if (reload) {
        g_Settings->sync();
    }
    return g_Settings->value(pathInfo, QVariant(QString(""))).toString();
}

void MultimediaPersistent::setMultimediaType(const DeviceWatcherType deviceWatcherType, const MultimediaType multimediaType)
{
    QString path("");
    switch (deviceWatcherType) {
    case DWT_SDDisk: {
        path = SDMultimedia;
        break;
    }
    case DWT_USBDisk: {
        path = USBMultimedia;
        break;
    }
    default: {
        return;
        break;
    }
    }
    g_Settings->setValue(path, multimediaType);
    g_Settings->sync();
}

void MultimediaPersistent::setMusicMillesmial(const DeviceWatcherType type, const int elapsed)
{
    switch (type) {
    case DWT_SDDisk: {
        g_Settings->setValue(SDMusicMillesmial, elapsed);
        break;
    }
    case DWT_USBDisk: {
        g_Settings->setValue(USBMusicMillesmial, elapsed);
        break;
    }
    default: {
        break;
    }
    }
    g_Settings->sync();
}

void MultimediaPersistent::setMusicPathInfo(const DeviceWatcherType type, const QString &pathInfo)
{
    switch (type) {
    case DWT_SDDisk: {
        g_Settings->setValue(SDMusicPathInfo, pathInfo);
        break;
    }
    case DWT_USBDisk: {
        g_Settings->setValue(USBMusicPathInfo, pathInfo);
        break;
    }
    default: {
        break;
    }
    }
    g_Settings->sync();
}

void MultimediaPersistent::setImagePathInfo(const DeviceWatcherType type, const QString &pathInfo)
{
    switch (type) {
    case DWT_SDDisk: {
        g_Settings->setValue(SDImagePathInfo, pathInfo);
        break;
    }
    case DWT_USBDisk: {
        g_Settings->setValue(USBImagePathInfo, pathInfo);
        break;
    }
    default: {
        break;
    }
    }
    g_Settings->sync();
}

void MultimediaPersistent::setVideoMillesmial(const DeviceWatcherType type, const int elapsed)
{
//    switch (type) {
//    case DWT_SDDisk: {
//        g_Settings->setValue(SDVideoMillesmial, elapsed);
//        break;
//    }
//    case DWT_USBDisk: {
//        g_Settings->setValue(USBVideoMillesmial, elapsed);
//        break;
//    }
//    default: {
//        break;
//    }
//    }
//    g_Settings->sync();
}

void MultimediaPersistent::setVideoPathInfo(const DeviceWatcherType type, const QString &pathInfo)
{
    switch (type) {
    case DWT_SDDisk: {
        g_Settings->setValue(SDVideoPathInfo, pathInfo);
        break;
    }
    case DWT_USBDisk: {
        g_Settings->setValue(USBVideoPathInfo, pathInfo);
        break;
    }
    default: {
        break;
    }
    }
    g_Settings->sync();
}

int MultimediaPersistent::getMultimediaType(const DeviceWatcherType deviceWatcherType, const bool reload)
{
    QString pathInfo("");
    switch (deviceWatcherType) {
    case DWT_SDDisk: {
        pathInfo = SDMultimedia;
        break;
    }
    case DWT_USBDisk: {
        pathInfo = USBMultimedia;
        break;
    }
    default: {
        return 0;
        break;
    }
    }
    if (reload) {
        g_Settings->sync();
    }
    bool flag(false);
    int ret = g_Settings->value(pathInfo, QVariant(0)).toInt(&flag);
    if (!flag) {
        ret = 0;
    }
    return ret;
}

void MultimediaPersistent::reset()
{
    g_Settings->clear();
    g_Settings->sync();
}
