#ifndef CARLIFELINKWIDGET_H
#define CARLIFELINKWIDGET_H

#include "BusinessLogic/Link.h"
#include "BusinessLogic/Widget.h"
#include "MessageBox.h"
#include <QWidget>
#include <QScopedPointer>

class CarlifeLinkWidgetPrivate;
class CarlifeLinkWidget
        : private QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(CarlifeLinkWidget)
public:
    explicit CarlifeLinkWidget(QWidget *parent = NULL);
    ~CarlifeLinkWidget();
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
    friend class CarlifeLinkWidgetPrivate;
    QScopedPointer<CarlifeLinkWidgetPrivate> m_Private;
};

#endif // CARLIFELINKWIDGET_H
