#ifndef AUXWIDGET_H
#define AUXWIDGET_H

#include "BusinessLogic/Widget.h"
#include "BusinessLogic/Setting.h"
#include <QWidget>

class AUXWidgetPrivate;
class AUXWidget
        : private QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(AUXWidget)
public:
    explicit AUXWidget(QWidget *parent = NULL);
    ~AUXWidget();
protected:
    void resizeEvent(QResizeEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void paintEvent(QPaintEvent* event);
protected slots:
    void onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status);
    void onAVStatus(const int type, const int status);
private slots:
    void onTimeout();
private:
    Q_DECLARE_PRIVATE(AUXWidget)
    AUXWidgetPrivate* const d_ptr;
};

#endif // AUXWIDGET_H
