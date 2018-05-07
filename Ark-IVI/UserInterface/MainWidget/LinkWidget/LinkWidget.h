#ifndef LINKWIDGET_H
#define LINKWIDGET_H

#include "BusinessLogic/Widget.h"
#include "BusinessLogic/Link.h"
#include "BusinessLogic/Setting.h"
#include <QWidget>
#include <QScopedPointer>

class LinkWidgetPrivate;
class LinkWidget
        : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(LinkWidget)
public:
    explicit LinkWidget(QWidget *parent = NULL);
    ~LinkWidget();
protected:
    void resizeEvent(QResizeEvent* event);
protected slots:
    void onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status);
    void onLinkStatusChange(const int type, const int status);
    void onArkProtocol(const ArkProtocolWrapper& protocol);
private slots:
    void onToolButtonRelease();
private:
    friend class LinkWidgetPrivate;
    QScopedPointer<LinkWidgetPrivate> m_Private;
};

#endif // LINKWIDGET_H
