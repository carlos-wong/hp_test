#ifndef RADIOUTILITYWIDGET_H
#define RADIOUTILITYWIDGET_H

#include "BusinessLogic/Radio.h"
#include <QWidget>

class RadioUtilityWidgetPrivate;
class RadioUtilityWidget
        : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(RadioUtilityWidget)
public:
    explicit RadioUtilityWidget(QWidget *parent = NULL);
    ~RadioUtilityWidget();
public slots:
    void onSliderMoved(int value);
protected:
    void resizeEvent(QResizeEvent* event);
protected slots:
    void onUsedStereoChange(const RadioStereoType type);
    void onListenStereoChange(const RadioStereoType type);
    void onLocChange(const RadioLocType type);
    void onRadioInfoChange(const RadioBandType type, const unsigned short frequency, const unsigned short index, const unsigned short min, const unsigned short max, const unsigned short step);
private slots:
    void onToolButtonRelease();
    void onToolButtonLongPressed();
private:
    Q_DECLARE_PRIVATE(RadioUtilityWidget)
    RadioUtilityWidgetPrivate* const d_ptr;
};

#endif // RADIOUTILITYWIDGET_H
