#ifndef IDLEWIDGET_H
#define IDLEWIDGET_H

#include "BusinessLogic/Setting.h"
#include <QWidget>
#include <QScopedPointer>

class IdleWidgetPrivate;
class IdleWidget
        : protected QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(IdleWidget)
public:
    explicit IdleWidget(QWidget *parent = NULL);
    ~IdleWidget();
protected:
    void paintEvent(QPaintEvent *event);
    void showEvent(QShowEvent* event);
    void resizeEvent(QResizeEvent* event);
    void mouseReleaseEvent(QMouseEvent *event);
protected slots:
    void onDisplayScreenChange(const int type);
    //    void onDisplayScreenChange(const DisplayScreenType type);
private:
    friend class IdleWidgetPrivate;
    QScopedPointer<IdleWidgetPrivate> m_Private;
};

#endif // IDLEWIDGET_H
