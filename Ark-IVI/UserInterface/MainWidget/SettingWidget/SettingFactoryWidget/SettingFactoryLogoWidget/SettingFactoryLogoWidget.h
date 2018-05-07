#ifndef SETTINGFACTORYLOGOWIDGET_H
#define SETTINGFACTORYLOGOWIDGET_H

#include "BusinessLogic/Widget.h"
#include <QWidget>

class SettingFactoryLogoWidgetPrivate;
class SettingFactoryLogoWidget
        : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(SettingFactoryLogoWidget)
public:
    explicit SettingFactoryLogoWidget(QWidget *parent = NULL);
    ~SettingFactoryLogoWidget();
    void hideDialog();
protected:
    void resizeEvent(QResizeEvent* event);
private slots:
    void onToolButtonRelease();
    void onConfirm();
    void onCancel();
private:
    Q_DECLARE_PRIVATE(SettingFactoryLogoWidget)
    SettingFactoryLogoWidgetPrivate* const d_ptr;
};

#endif // SETTINGFACTORYLOGOWIDGET_H
