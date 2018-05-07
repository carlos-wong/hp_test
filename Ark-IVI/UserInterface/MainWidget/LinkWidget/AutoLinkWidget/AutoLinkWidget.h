#ifndef AUTOLINKWIDGET_H
#define AUTOLINKWIDGET_H

#include "BusinessLogic/Link.h"
#include "BusinessLogic/Widget.h"
#include <QWidget>
#include <QScopedPointer>

class AutoLinkWidgetPrivate;
class AutoLinkWidget
        : private QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(AutoLinkWidget)
public:
    explicit AutoLinkWidget(QWidget *parent = NULL);
    ~AutoLinkWidget();
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
    friend class AutoLinkWidgetPrivate;
    QScopedPointer<AutoLinkWidgetPrivate> m_Private;
};

#endif // AUTOLINKWIDGET_H
