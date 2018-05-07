#include "RadioPersistent.h"
#include <QSettings>
#include <stdlib.h>
#include <QApplication>
#include <QFile>
#include <QDebug>
#include <QMap>

#ifdef gcc
static const QString persistentPath("/tmp/Radio.ini");
static const QString persistentPathBackup("/tmp/RadioBackup.ini");
#else
static const QString persistentPath("/data/Radio.ini");
static const QString persistentPathBackup("/data/RadioBackup.ini");
#endif
static const QString Exists("Exists");
static const QString Band("Band");
static const QString Stereo("Stereo");
static const QString Loc("Loc");
static const QString FM1FrequencyList("FM1FrequencyList");
//static const QString FM2FrequencyList("FM2FrequencyList");
//static const QString FM3FrequencyList("FM3FrequencyList");
static const QString AM1FrequencyList("AM1FrequencyList");
//static const QString AM2FrequencyList("AM2FrequencyList");
static const QString FM1FrequencyListIndex("FM1FrequencyListIndex");
//static const QString FM2FrequencyListIndex("FM2FrequencyListIndex");
//static const QString FM3FrequencyListIndex("FM3FrequencyListIndex");
static const QString AM1FrequencyListIndex("AM1FrequencyListIndex");
//static const QString AM2FrequencyListIndex("AM2FrequencyListIndex");
static const QString FM1Frequency("FM1Frequency");
//static const QString FM2Frequency("FM2Frequency");
//static const QString FM3Frequency("FM3Frequency");
static const QString AM1Frequency("AM1Frequency");
//static const QString AM2Frequency("AM2Frequency");

//enum Area {
//    A_Euro = 0,
//    A_USA1,
//    A_USA2,
//    A_Russ,
//    A_Japan,
//    A_Size,
//};
//static Area m_Area = A_Euro;
//struct AreaRadioInfo {
//    //    Area area;
//    unsigned short fmStep;
//    unsigned short fmMinFreq;
//    unsigned short fmMaxFreq;
//    unsigned short amStep;
//    unsigned short amMinFreq;
//    unsigned short amMaxFreq;
//};

//static struct AreaRadioInfo m_AreaRadioInfo[A_Size] = {
//{
//    //    .area = A_Euro,
//    .fmStep = 10,
//    .fmMinFreq = 8750,
//    .fmMaxFreq = 10800,
//    .amStep = 9,
//    .amMinFreq = 522,
//    .amMaxFreq = 1620,
//},
//{
//    //    .area = A_USA1,
//    .fmStep = 20,
//    .fmMinFreq = 8750,
//    .fmMaxFreq = 10790,
//    .amStep = 10,
//    .amMinFreq = 530,
//    .amMaxFreq = 1710,
//},
//{
//    //    .area = A_USA2,
//    .fmStep = 10,
//    .fmMinFreq = 8750,
//    .fmMaxFreq = 10800,
//    .amStep = 10,
//    .amMinFreq = 530,
//    .amMaxFreq = 1710,
//},
//{
//    //    .area = A_Russ,
//    .fmStep = 3,
//    .fmMinFreq = 6500,
//    .fmMaxFreq = 7400,
//    .amStep = 9,
//    .amMinFreq = 522,
//    .amMaxFreq = 1620,
//},
//{
//    //    .area = A_Japan,
//    .fmStep = 1,
//    .fmMinFreq = 7600,
//    .fmMaxFreq = 9000,
//    .amStep = 9,
//    .amMinFreq = 522,
//    .amMaxFreq = 1629,
//}
//};

class RadioSettings : public QSettings
{
    Q_DISABLE_COPY(RadioSettings)
#ifdef g_Settings
#undef g_Settings
#endif
#define g_Settings (RadioSettings::instance())
public:
    inline static RadioSettings* instance() {
        static RadioSettings* settings(new RadioSettings(qApp));
        return settings;
    }
    void clear();
    void sync();
    void setValue(const QString &key, const QVariant &value);
private:
    explicit RadioSettings(QObject* parent = NULL);
    ~RadioSettings();
    QSettings m_SettingsBackup;
};

void RadioPersistent::reset()
{
    g_Settings->clear();
    g_Settings->sync();
}

unsigned short RadioPersistent::getMinFrequency(const RadioBandType type)
{
    unsigned short ret(0);
    switch (type) {
    case RBT_FM1:
/*    case RBT_FM2:
    case RBT_FM3:*/ {
//        ret = m_AreaRadioInfo[m_Area].fmMinFreq;
        break;
    }
    case RBT_AM1:
/*    case RBT_AM2:*/ {
//        ret = m_AreaRadioInfo[m_Area].amMinFreq;
        break;
    }
    default: {
        break;
    }
    }
    return ret;
}

unsigned short RadioPersistent::getMaxFrequency(const RadioBandType type)
{
    unsigned short ret(0);
    switch (type) {
    case RBT_FM1:
/*    case RBT_FM2:
    case RBT_FM3:*/ {
//        ret = m_AreaRadioInfo[m_Area].fmMaxFreq;
        break;
    }
    case RBT_AM1:
/*    case RBT_AM2:*/ {
//        ret = m_AreaRadioInfo[m_Area].amMaxFreq;
        break;
    }
    default: {
        break;
    }
    }
    return ret;
}

unsigned short RadioPersistent::getStep(const RadioBandType type)
{
    unsigned short ret(0);
    switch (type) {
    case RBT_FM1:
/*    case RBT_FM2:
    case RBT_FM3:*/ {
//        ret = m_AreaRadioInfo[m_Area].fmStep;
        break;
    }
    case RBT_AM1:
/*    case RBT_AM2:*/ {
//        ret = m_AreaRadioInfo[m_Area].amStep;
        break;
    }
    default: {
        break;
    }
    }
    return ret;
}

void RadioPersistent::setBand(const RadioBandType type)
{
    g_Settings->setValue(Band, type);
    g_Settings->sync();
}

RadioBandType RadioPersistent::getBand(const bool reload)
{
    RadioBandType defaultValue(RBT_FM1);
    if (reload) {
        g_Settings->sync();
    }
    bool flag(false);
    RadioBandType ret = static_cast<RadioBandType>(g_Settings->value(Band, defaultValue).toInt(&flag));
    if (!flag) {
        ret = defaultValue;
    }
    return ret;
}

void RadioPersistent::setFrequencyList(const RadioBandType type, const QList<QVariant> &list)
{
    switch (type) {
    case RBT_FM1: {
        g_Settings->setValue(FM1FrequencyList, list);
        break;
    }
//    case RBT_FM2: {
//        g_Settings->setValue(FM2FrequencyList, list);
//        break;
//    }
//    case RBT_FM3: {
//        g_Settings->setValue(FM3FrequencyList, list);
//        break;
//    }
    case RBT_AM1: {
        g_Settings->setValue(AM1FrequencyList, list);
        break;
    }
//    case RBT_AM2: {
//        g_Settings->setValue(AM2FrequencyList, list);
//        break;
//    }
    default: {
        break;
    }
    }
    g_Settings->sync();
}

QList<QVariant> RadioPersistent::getFrequencyList(const RadioBandType type, const bool reload)
{
    QList<QVariant> defaultValue;
    defaultValue.clear();
    if (reload) {
        g_Settings->sync();
    }
    QString path("");
    switch (type) {
    case RBT_FM1: {
        defaultValue.append(8750);
        defaultValue.append(9010);
        defaultValue.append(9810);
        defaultValue.append(10610);
        defaultValue.append(10800);
        defaultValue.append(8750);
        path = FM1FrequencyList;
        break;
    }
//    case RBT_FM2: {
//        path = FM2FrequencyList;
//        break;
//    }
//    case RBT_FM3: {
//        path = FM3FrequencyList;
//        break;
//    }
    case RBT_AM1: {
        defaultValue.append(522);
        defaultValue.append(603);
        defaultValue.append(999);
        defaultValue.append(1404);
        defaultValue.append(1620);
        defaultValue.append(522);
        path = AM1FrequencyList;
        break;
    }
//    case RBT_AM2: {
//        break;
//    }
    default: {
        break;
    }
    }
    return g_Settings->value(path, defaultValue).toList();
}

void RadioPersistent::setFrequencyListIndex(const RadioBandType type, const unsigned short index)
{
    switch (type) {
    case RBT_FM1: {
        g_Settings->setValue(FM1FrequencyListIndex, index);
        break;
    }
//    case RBT_FM2: {
//        g_Settings->setValue(FM2FrequencyListIndex, index);
//        break;
//    }
//    case RBT_FM3: {
//        g_Settings->setValue(FM3FrequencyListIndex, index);
//        break;
//    }
    default: {
        break;
    }
    }
    g_Settings->sync();
}

unsigned short RadioPersistent::getFrequencyListIndex(const RadioBandType type, const bool reload)
{
    if (reload) {
        g_Settings->sync();
    }
    QString pathInfo("");
    switch (type) {
    case RBT_FM1: {
        pathInfo = FM1FrequencyListIndex;
        break;
    }
//    case RBT_FM2: {
//        pathInfo = FM2FrequencyListIndex;
//        break;
//    }
//    case RBT_FM3: {
//        pathInfo = FM3FrequencyListIndex;
//        break;
//    }
    case RBT_AM1: {
        pathInfo = AM1FrequencyListIndex;
        break;
    }
//    case RBT_AM2: {
//        pathInfo = AM2FrequencyListIndex;
//        break;
//    }
    default: {
        break;
    }
    }
    unsigned short defaultValue(0);
    unsigned short ret = g_Settings->value(Stereo, defaultValue).toInt();
    return ret;
}

void RadioPersistent::setStereo(const RadioStereoType type)
{
    g_Settings->setValue(Stereo, type);
    g_Settings->sync();
}

void RadioPersistent::setFrequency(const RadioBandType type, const unsigned short frequency)
{
    QString path("");
    switch (type) {
    case RBT_FM1: {
        path = FM1Frequency;
        break;
    }
//    case RBT_FM2: {
//        path = FM2Frequency;
//        break;
//    }
//    case RBT_FM3: {
//        path = FM3Frequency;
//        break;
//    }
    case RBT_AM1: {
        path = AM1Frequency;
        break;
    }
//    case RBT_AM2: {
//        path = AM2Frequency;
//        break;
//    }
    default: {
        break;
    }
    }
    g_Settings->setValue(path, frequency);
    g_Settings->sync();
}

unsigned short RadioPersistent::getFrequency(const RadioBandType type, const bool reload)
{
    if (reload) {
        g_Settings->sync();
    }
    QString pathInfo("");
    switch (type) {
    case RBT_FM1: {
        pathInfo = FM1Frequency;
        break;
    }
//    case RBT_FM2: {
//        pathInfo = FM2Frequency;
//        break;
//    }
//    case RBT_FM3: {
//        pathInfo = FM3Frequency;
//        break;
//    }
    case RBT_AM1: {
        pathInfo = AM1Frequency;
        break;
    }
//    case RBT_AM2: {
//        pathInfo = AM2Frequency;
//        break;
//    }
    default: {
        break;
    }
    }
    unsigned short defaultValue(8750);
    unsigned short ret = g_Settings->value(pathInfo, defaultValue).toInt();
    return ret;
}

RadioStereoType RadioPersistent::getStereo(const bool reload)
{
    RadioStereoType defaultValue(RST_Mono);
    if (reload) {
        g_Settings->sync();
    }
    bool flag(false);
    RadioStereoType ret = static_cast<RadioStereoType>(g_Settings->value(Stereo, defaultValue).toInt(&flag));
    if (!flag) {
        ret = defaultValue;
    }
    return ret;
}

void RadioPersistent::setLoc(const RadioLocType type)
{
    g_Settings->setValue(Loc, type);
    g_Settings->sync();
}

RadioLocType RadioPersistent::getLoc(const bool reload)
{
    RadioLocType defaultValue(RLT_LocalSensitivityOff);
    if (reload) {
        g_Settings->sync();
    }
    bool flag(false);
    RadioLocType ret = static_cast<RadioLocType>(g_Settings->value(Loc, defaultValue).toInt(&flag));
    if (!flag) {
        ret = defaultValue;
    }
    return ret;
}

RadioPersistent::RadioPersistent()
{
}

RadioPersistent::~RadioPersistent()
{
}

void RadioSettings::clear()
{
    QSettings::clear();
    m_SettingsBackup.clear();
}

void RadioSettings::sync()
{
    QSettings::sync();
    m_SettingsBackup.sync();
}

void RadioSettings::setValue(const QString &key, const QVariant &value)
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

RadioSettings::RadioSettings(QObject *parent)
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

RadioSettings::~RadioSettings()
{
}
