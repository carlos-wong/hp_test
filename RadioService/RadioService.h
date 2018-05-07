#ifndef RADIOSERVICE_H
#define RADIOSERVICE_H

#include "ArkProtocolWrapper.h"
#include "RadioPersistent.h"
#include <QObject>
#include <QScopedPointer>

class RadioServicePrivate;
class RadioService : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(RadioService)
public:
    explicit RadioService(QObject* parent = NULL);
    ~RadioService();
    void setStereo(const RadioStereoType type);
    void toggleStereo();
    void setLoc(const RadioLocType type);
    void toggleLoc();
    void setBand(const RadioBandType type);
    void toggleBand();
    void setCurrentBandTurnAdd();
    void setCurrentBandTurnSub();
    void previousFrequencyListIndex(const RadioBandType type);
    void previousCurrentBandFrequencyListIndex();
    void nextFrequencyListIndex(const RadioBandType type);
    void nextCurrentBandFrequencyListIndex();
    void setFrequency(const RadioBandType type, const unsigned short frequency);
    void setCurrentBandFrequency(const unsigned short frequency);
    void setCurrentBandIndex(const unsigned short index);
    void setIndex(const RadioBandType type, const unsigned short index);
    void setSearchPreviousStation();
    void setSearchNextStation();
    void setSearch(const RadioBandType type);
    void setCurrentBandSearch();
    void setBrowse(const RadioBandType type);
    void setCurrentBandBrowse();
signals:
    void onRadioInfoChange(const RadioBandType type, const unsigned short frequency, const unsigned short index);
    void onBandChange(const RadioBandType band, const QString& xml);
    void onStereoChange(const RadioStereoType type);
    void onLocChange(const RadioLocType type);
private slots:
    void onArkProtocol(const ArkProtocolWrapper& protocol);
//    void onTimeout();
private:
    friend class RadioServicePrivate;
    QScopedPointer<RadioServicePrivate> m_Private;
};

#endif // RADIOSERVICE_H
