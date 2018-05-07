#ifndef SETTINGLINKWIDGET_H
#define SETTINGLINKWIDGET_H

#include "BusinessLogic/Widget.h"
#include <QWidget>

class SettingLinkWidgetPrivate;
class SettingLinkWidget
        : private QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(SettingLinkWidget)
public:
    explicit SettingLinkWidget(QWidget *parent = nullptr);
    ~SettingLinkWidget();
protected:
    void resizeEvent(QResizeEvent* event);
protected slots:
    void onWidgetTypeChange(const Widget::Type desitinationType, const Widget::Type requestType, const QString &status);
private:
    Q_DECLARE_PRIVATE(SettingLinkWidget)
    SettingLinkWidgetPrivate* const d_ptr;
};

#endif // SETTINGLINKWIDGET_H
