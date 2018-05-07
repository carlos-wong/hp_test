#ifndef SETTINGFACTORYLINKWIDGET_H
#define SETTINGFACTORYLINKWIDGET_H

#include "BusinessLogic/Setting.h"
#include <QWidget>

class SettingFactoryLinkWidgetPrivate;
class SettingFactoryLinkWidget
        : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(SettingFactoryLinkWidget)
public:
    explicit SettingFactoryLinkWidget(QWidget* parent = NULL);
    ~SettingFactoryLinkWidget();
    void hideDialog();
protected:
    void resizeEvent(QResizeEvent* event);
private slots:
    void onListViewItemRelease(const int index);
    void onConfirm();
    void onCancel();
private:
    Q_DECLARE_PRIVATE(SettingFactoryLinkWidget)
    SettingFactoryLinkWidgetPrivate* const d_ptr;
};

#endif // SETTINGFACTORYLINKWIDGET_H
