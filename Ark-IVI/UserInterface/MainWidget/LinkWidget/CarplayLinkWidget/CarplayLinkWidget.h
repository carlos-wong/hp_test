#ifndef CARPLAYLINKWIDGET_H
#define CARPLAYLINKWIDGET_H

#include "BusinessLogic/Link.h"
#include "BusinessLogic/Widget.h"
#include <QWidget>
#include <QScopedPointer>

class CarplayLinkWidgetPrivate;
class CarplayLinkWidget
        : private QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(CarplayLinkWidget)
public:
    explicit CarplayLinkWidget(QWidget *parent = NULL);
    ~CarplayLinkWidget();
protected:
    void resizeEvent(QResizeEvent* event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    bool event(QEvent* event);
protected slots:
    void onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status);
    void onLinkStatusChange(const int type, const int status);
private:
    friend class CarplayLinkWidgetPrivate;
    QScopedPointer<CarplayLinkWidgetPrivate> m_Private;
};

#endif // CARPLAYLINKWIDGET_H
