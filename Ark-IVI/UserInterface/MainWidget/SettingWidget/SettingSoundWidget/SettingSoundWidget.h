#ifndef SETTINGSOUNDWIDGET_H
#define SETTINGSOUNDWIDGET_H

#include "SettingSoundToolWidget/SettingSoundToolWidget.h"
#include "BusinessLogic/Widget.h"
#include <QWidget>

class SettingSoundWidgetPrivate;
class SettingSoundWidget
        : private QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(SettingSoundWidget)
public:
    explicit SettingSoundWidget(QWidget *parent = NULL);
    ~SettingSoundWidget();
protected:
    void resizeEvent(QResizeEvent* event);
protected slots:
    void onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status);
private slots:
    void onToolBtnRelease(const SettingSoundToolWidget::CheckBtn btn);
private:
    Q_DECLARE_PRIVATE(SettingSoundWidget)
    SettingSoundWidgetPrivate* const d_ptr;
};

#endif // SETTINGSOUNDWIDGET_H
