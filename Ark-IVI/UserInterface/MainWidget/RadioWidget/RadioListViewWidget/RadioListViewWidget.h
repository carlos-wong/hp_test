#ifndef RADIOLISTVIEWWIDGET_H
#define RADIOLISTVIEWWIDGET_H

#include "BusinessLogic/Radio.h"
#include "BusinessLogic/Widget.h"
#include <QWidget>

class RadioListViewWidgetPrivate;
class RadioListViewWidget
        : private QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(RadioListViewWidget)
public:
    explicit RadioListViewWidget(QWidget *parent = NULL);
    ~RadioListViewWidget();
protected:
    void resizeEvent(QResizeEvent* event);
protected slots:
    void onRadioInfoChange(const RadioBandType type, const unsigned short frequency, const unsigned short index, const unsigned short min, const unsigned short max, const unsigned short step);
    void onRadioListChange(const QList<unsigned short>& list);
private slots:
    void onListViewItemRelease();
    void onListViewItemLongPress();
private:
    Q_DECLARE_PRIVATE(RadioListViewWidget)
    RadioListViewWidgetPrivate* const d_ptr;
};

#endif // RADIOLISTVIEWWIDGET_H
