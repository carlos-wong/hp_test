#ifndef UTILITYWIDGET_H
#define UTILITYWIDGET_H

#include "BusinessLogic/Widget.h"
#include <QWidget>
#include <QDateTime>

class UtilityWidgetPrivate;
class UtilityWidget
        : private QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(UtilityWidget)
public:
    explicit UtilityWidget(QWidget *parent = NULL);
    ~UtilityWidget();
protected:
    void timerEvent(QTimerEvent* event);
    void resizeEvent(QResizeEvent* event);
protected slots:
    void onCurrentDateTime(const QDateTime &dateTime);
private:
    Q_DECLARE_PRIVATE(UtilityWidget)
    UtilityWidgetPrivate* const d_ptr;
};

#endif // UTILITYWIDGET_H
