#ifndef SETTINGFACTORYWIDGET_H
#define SETTINGFACTORYWIDGET_H

#include "SettingFactoryToolWidget/SettingFactoryToolWidget.h"
#include "BusinessLogic/Widget.h"
#include <QWidget>

class SettingFactoryWidgetPrivate;
class SettingFactoryWidget
        : private QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(SettingFactoryWidget)
public:
    explicit SettingFactoryWidget(QWidget *parent = NULL);
    ~SettingFactoryWidget();
protected:
    void resizeEvent(QResizeEvent* event);
protected slots:
    void onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status);
private slots:
    void onToolBtnRelease(const SettingFactoryToolWidget::CheckBtn btn);
    void onNumberPadKeyType(const QString& type);
private:
    Q_DECLARE_PRIVATE(SettingFactoryWidget)
    SettingFactoryWidgetPrivate* const d_ptr;
};

#endif // SETTINGFACTORYWIDGET_H
