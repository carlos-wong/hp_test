#ifndef RADIOMINIWIDGET_H
#define RADIOMINIWIDGET_H

#include "BusinessLogic/Radio.h"
#include <QWidget>

class RadioMiniWidgetPrivate;
class RadioMiniWidget
        : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(RadioMiniWidget)
public:
    explicit RadioMiniWidget(QWidget *parent = NULL);
    ~RadioMiniWidget();
protected:
    void resizeEvent(QResizeEvent* event);
    void paintEvent(QPaintEvent *);
protected slots:
    void onRadioInfoChange(const RadioBandType type, const unsigned short frequency, const unsigned short index, const unsigned short min, const unsigned short max, const unsigned short step);
private slots:
    void onBmpButtonRelease();
private:
    Q_DECLARE_PRIVATE(RadioMiniWidget)
    RadioMiniWidgetPrivate* const d_ptr;
};

#endif // RADIOMINIWIDGET_H
