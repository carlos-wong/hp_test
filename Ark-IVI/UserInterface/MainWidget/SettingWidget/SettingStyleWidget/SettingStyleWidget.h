#ifndef SETTINGSTYLEWIDGET_H
#define SETTINGSTYLEWIDGET_H

#include "BusinessLogic/Widget.h"
#include "SettingStyleToolWidget/SettingStyleToolWidget.h"
#include <QWidget>

class SettingStyleWidgetPrivate;
class SettingStyleWidget
        : private QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(SettingStyleWidget)
public:
    explicit SettingStyleWidget(QWidget *parent = NULL);
    ~SettingStyleWidget();
protected:
    void resizeEvent(QResizeEvent* event);
protected slots:
    void onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status);
private slots:
    void onToolBtnRelease(const SettingStyleToolWidget::CheckBtn btn);
private:
    Q_DECLARE_PRIVATE(SettingStyleWidget)
    SettingStyleWidgetPrivate* const d_ptr;
};

#endif // SETTINGSTYLEWIDGET_H
