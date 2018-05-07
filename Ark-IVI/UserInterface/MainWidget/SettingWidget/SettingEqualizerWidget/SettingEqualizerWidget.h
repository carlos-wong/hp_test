#ifndef SETTINGEQUALIZERWIDGET_H
#define SETTINGEQUALIZERWIDGET_H

#include "BusinessLogic/Widget.h"
#include <QWidget>

class SettingEqualizerWidgetPrivate;
class SettingEqualizerWidget
        : private QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(SettingEqualizerWidget)
public:
    explicit SettingEqualizerWidget(QWidget *parent = NULL);
    ~SettingEqualizerWidget();
protected:
    void resizeEvent(QResizeEvent* event);
protected slots:
    void onWidgetTypeChange(const Widget::Type desitinationType, const Widget::Type requestType, const QString &status);
private:
    Q_DECLARE_PRIVATE(SettingEqualizerWidget)
    SettingEqualizerWidgetPrivate* const d_ptr;
};

#endif // SETTINGEQUALIZERWIDGET_H
