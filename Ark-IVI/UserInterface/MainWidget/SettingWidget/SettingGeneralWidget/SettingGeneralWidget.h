#ifndef SETTINGGENERALWIDGET_H
#define SETTINGGENERALWIDGET_H

#include "BusinessLogic/Widget.h"
#include "SettingGeneralToolWidget/SettingGeneralToolWidget.h"
#include <QWidget>

class SettingGeneralWidgetPrivate;
class SettingGeneralWidget
        : private QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(SettingGeneralWidget)
public:
    explicit SettingGeneralWidget(QWidget* parent = NULL);
    ~SettingGeneralWidget();
protected:
    void resizeEvent(QResizeEvent* event);
protected slots:
    void onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status);
private slots:
    void onToolBtnRelease(const SettingGeneralToolWidget::CheckBtn btn);
private:
    Q_DECLARE_PRIVATE(SettingGeneralWidget)
    SettingGeneralWidgetPrivate* const d_ptr;
};

#endif // SETTINGGENERALWIDGET_H
