#ifndef RADIOTOOLWIDGET_H
#define RADIOTOOLWIDGET_H

#include "BusinessLogic/Radio.h"
#include <QWidget>

class RadioToolWidgetPrivate;
class RadioToolWidget
        : private QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(RadioToolWidget)
public:
    explicit RadioToolWidget(QWidget *parent = NULL);
    ~RadioToolWidget();
protected:
    void resizeEvent(QResizeEvent* event);
private slots:
    void onToolButtonRelease();
    void onRadioInfoChange(const RadioBandType type, const unsigned short frequency,
                                                   const unsigned short index, const unsigned short min, const unsigned short max, const unsigned short step);
private:
    Q_DECLARE_PRIVATE(RadioToolWidget)
    RadioToolWidgetPrivate* const d_ptr;
};

#endif // RADIOTOOLWIDGET_H
