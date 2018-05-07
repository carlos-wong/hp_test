#ifndef SETTINGWIDGET_H
#define SETTINGWIDGET_H

#include "BusinessLogic/Widget.h"
#include <QWidget>

class SettingWidgetPrivate;
class SettingWidget
        : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(SettingWidget)
public:
    explicit SettingWidget(QWidget *parent = NULL);
    ~SettingWidget();
protected:
    void resizeEvent(QResizeEvent* event);
protected slots:
    void onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status);
private:
    Q_DECLARE_PRIVATE(SettingWidget)
    SettingWidgetPrivate* const d_ptr;
};

#endif // SETTINGWIDGET_H
