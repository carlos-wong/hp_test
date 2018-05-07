#ifndef MIRRORLINKWIDGET_H
#define MIRRORLINKWIDGET_H

#include "BusinessLogic/Link.h"
#include "BusinessLogic/Widget.h"
#include "MessageBox.h"
#include <QWidget>
#include <QScopedPointer>

class MirrorLinkWidgetPrivate;
class MirrorLinkWidget
        : private QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(MirrorLinkWidget)
public:
    explicit MirrorLinkWidget(QWidget *parent = NULL);
    ~MirrorLinkWidget();
protected:
    void resizeEvent(QResizeEvent* event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    bool event(QEvent* event);
protected slots:
    void onWidgetTypeChange(const Widget::Type desitinationType, const Widget::Type requestType, const QString &status);
    void onLinkStatusChange(const int type, const int status);
    //  void onLinkStatusChange(const Link_Type type, const Link_STATUS status);
private slots:
    void onPopBtnRelease();
    void onToolBtnRelease(const int type);
    void onTimeout();
private:
    friend class MirrorLinkWidgetPrivate;
    QScopedPointer<MirrorLinkWidgetPrivate> m_Private;
};

#endif // MIRRORLINKWIDGET_H
