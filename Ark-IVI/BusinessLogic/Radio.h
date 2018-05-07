#ifndef RADIO_H
#define RADIO_H

#include "RadioService.h"
#include <QList>
#include <QCoreApplication>
#include <QObject>
#include <QByteArray>

class RadioPrivate;
class Radio : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(Radio)
#ifdef g_Radio
#undef g_Radio
#endif
#define g_Radio (Radio::instance())
public:
    inline static Radio* instance() {
        static Radio* radio(new Radio(qApp));
        return radio;
    }
    bool isStartComplete();
    void reset();
    void initialize();
    void toggleStereo();
    void toggleLoc();
    void toggleBand();
    void setCurrentBandTurnAdd();
    void setCurrentBandTurnSub();
    void setRadioIdle();
    void setCurrentBandFrequency(const unsigned short frequency);
    void setCurrentBandIndex(const unsigned short index);
    void setSearchPreviousStation();
    void setSearchNextStation();
    void setCurrentBandSearch();
    void setCurrentBandBrowse();
    void setCurrentBandFrequencyToIndex(const unsigned short index);
    int getCurrendBandIndex();
    int getCurrendBandFrequency();
    RadioWorkType getWorkType();
    RadioBandType getCurrentBand();
    RadioStereoType getUsedStereoType();
    RadioStereoType getListenStereoType();
    RadioLocType getLocType();
    unsigned short getCurrentBandMin();
    unsigned short getCurrentBandMax();
    unsigned short getCurrentBandStep();
    const QList<unsigned short>& getCurrentBandList();
    void setRadioSmeter(QByteArray& byteArray);
    QByteArray& getRadioSmeter();
signals:
    void startComplete();
    void onUsedStereoChange(const RadioStereoType type);
    void onListenStereoChange(const RadioStereoType type);
    void onLocChange(const RadioLocType type);
    void onRadioInfoChange(const RadioBandType type, const unsigned short frequency, const unsigned short index, const unsigned short min, const unsigned short max, const unsigned short step);
    void onRadioListChange(const QList<unsigned short>& list);
    void onRadioSmeter(const QByteArray& byteArray);
private slots:
    void onArkProtocol(const ArkProtocolWrapper& protocol);
    void onHolderChange(const int oldHolder, const int newHolder);
private:
    friend class Setting;
    friend class SettingPrivate;
    void setInnerRadio(const ArkRadio* radio);
    explicit Radio(QObject* parent = NULL);
    ~Radio();
    RadioPrivate* const d_ptr;
    Q_DECLARE_PRIVATE(Radio)
};

#endif // RADIO_H
