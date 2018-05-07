#ifndef HOMEWIDGET_H
#define HOMEWIDGET_H

#include "BusinessLogic/Widget.h"
#include <QWidget>

class HomeWidgetPrivate;
class HomeWidget
        : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(HomeWidget)
public:
    explicit HomeWidget(QWidget *parent = NULL);
    ~HomeWidget();
protected:
    void resizeEvent(QResizeEvent* event);
protected slots:
    void onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status);
    void onHolderChange(const int oldHolder, const int newHolder);
private:
    HomeWidgetPrivate* const d_ptr;
    Q_DECLARE_PRIVATE(HomeWidget)
};

#endif // HOMEWIDGET_H
