#ifndef TIMEWIDGET_H
#define TIMEWIDGET_H

#include "BusinessLogic/Widget.h"
#include <QWidget>
#include <QDateTime>

class TimeWidgetPrivate;
class TimeWidget
        : private QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(TimeWidget)
public:
    explicit TimeWidget(QWidget *parent = NULL);
    ~TimeWidget();
protected:
    void resizeEvent(QResizeEvent* event);
    void paintEvent(QPaintEvent* event);
protected slots:
    void onCurrentDateTime(const QDateTime &dateTime);
    void onHourClockChange(const int type);
private:
    Q_DECLARE_PRIVATE(TimeWidget)
    TimeWidgetPrivate* const d_ptr;
};

#endif // TIMEWIDGET_H
