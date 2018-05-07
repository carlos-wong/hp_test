#ifndef SETTINGTOOLBARWIDGET_H
#define SETTINGTOOLBARWIDGET_H

#include "BusinessLogic/Widget.h"
#include <QWidget>

class SettingToolBarWidgetPrivate;
class SettingToolBarWidget
        : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(SettingToolBarWidget)
public:
    explicit SettingToolBarWidget(QWidget* parent = NULL);
    ~SettingToolBarWidget();
protected:
    void resizeEvent(QResizeEvent* event);
protected slots:
    void onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status);
private slots:
    void onToolBarBtnRelease();
private:
    Q_DECLARE_PRIVATE(SettingToolBarWidget)
    SettingToolBarWidgetPrivate* const d_ptr;
};

#endif // SETTINGTOOLBARWIDGET_H
