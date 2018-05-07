#ifndef REVERSINGWIDGET_H
#define REVERSINGWIDGET_H

#include "BusinessLogic/Widget.h"
#include "BusinessLogic/Setting.h"
#include <QWidget>

class ReversingWidgetPrivate;
class ReversingWidget
        : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(ReversingWidget)
public:
    explicit ReversingWidget(QWidget *parent = NULL);
    ~ReversingWidget();
    void onArkCarHandler(const struct ArkCar* car);
protected:
    void resizeEvent(QResizeEvent* event);
private:
    Q_DECLARE_PRIVATE(ReversingWidget)
    ReversingWidgetPrivate* const d_ptr;
};

#endif // REVERSINGWIDGET_H
