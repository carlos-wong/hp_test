#ifndef STEERINGWIDGET_H
#define STEERINGWIDGET_H

#include "BusinessLogic/Setting.h"
#include "BusinessLogic/Widget.h"
#include <QWidget>

class SteeringWidgetPrivate;
class SteeringWidget
        : private QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(SteeringWidget)
public:
    explicit SteeringWidget(QWidget *parent = NULL);
    ~SteeringWidget();
protected:
    void resizeEvent(QResizeEvent* event);
protected slots:
    void onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status);
    void onSteeringStudyChange(const QMap<StudySteeringWheelStatus, struct StudySteeringWheel>& map);
private slots:
    void onToolButtonRelease();
private:
    Q_DECLARE_PRIVATE(SteeringWidget)
    SteeringWidgetPrivate* const d_ptr;
};

#endif // STEERINGWIDGET_H
