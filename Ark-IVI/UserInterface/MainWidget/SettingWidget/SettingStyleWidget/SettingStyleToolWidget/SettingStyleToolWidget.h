#ifndef SETTINGSTYLETOOLWIDGET_H
#define SETTINGSTYLETOOLWIDGET_H

#include "BusinessLogic/Widget.h"
#include <QWidget>

class SettingStyleToolWidgetPrivate;
class SettingStyleToolWidget
        : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(SettingStyleToolWidget)
public:
    enum CheckBtn {
        CB_Wallpaper = 0,
    };
    explicit SettingStyleToolWidget(QWidget *parent = NULL);
    ~SettingStyleToolWidget();
    void setCheckBtn(const SettingStyleToolWidget::CheckBtn btn);
protected:
    void resizeEvent(QResizeEvent* event);
signals:
    void toolButtonRelease(const SettingStyleToolWidget::CheckBtn btn);
private slots:
    void onToolButtonRelease();
private:
    Q_DECLARE_PRIVATE(SettingStyleToolWidget)
    SettingStyleToolWidgetPrivate* const d_ptr;
};

#endif // SETTINGSTYLETOOLWIDGET_H
