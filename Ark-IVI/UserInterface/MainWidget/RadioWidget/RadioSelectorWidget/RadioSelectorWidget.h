#ifndef RADIOSELECTORWIDGET_H
#define RADIOSELECTORWIDGET_H

#include "BusinessLogic/Radio.h"
#include "BusinessLogic/Widget.h"
#include <QWidget>

class RadioSelectorWidgetPrivate;
class RadioSelectorWidget
        : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(RadioSelectorWidget)
public:
    explicit RadioSelectorWidget(QWidget *parent = NULL);
    ~RadioSelectorWidget();
signals:
    void sliderMoved(int value);
protected:
    void resizeEvent(QResizeEvent* event);
protected slots:
    void onRadioInfoChange(const RadioBandType type, const unsigned short frequency, const unsigned short index, const unsigned short min, const unsigned short max, const unsigned short step);
private slots:
    void onTimeout();
    void onSliderMoved(int value);
    void onSliderReleased(int value);
private:
    Q_DECLARE_PRIVATE(RadioSelectorWidget)
    RadioSelectorWidgetPrivate* const d_ptr;
};

#endif // RADIOSELECTORWIDGET_H
