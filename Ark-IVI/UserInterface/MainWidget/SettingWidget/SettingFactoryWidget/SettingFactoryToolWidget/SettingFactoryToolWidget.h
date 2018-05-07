#ifndef SETTINGFACTORYTOOLWIDGET_H
#define SETTINGFACTORYTOOLWIDGET_H

#include <QWidget>

class SettingFactoryToolWidgetPrivate;
class SettingFactoryToolWidget : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(SettingFactoryToolWidget)
public:
    enum CheckBtn {
        CB_UpdateLink = 0,
        CB_UpdateLogo,
    };
    explicit SettingFactoryToolWidget(QWidget *parent = NULL);
    ~SettingFactoryToolWidget();
    void setCheckBtn(const SettingFactoryToolWidget::CheckBtn btn);
protected:
    void resizeEvent(QResizeEvent* event);
signals:
    void toolButtonRelease(const SettingFactoryToolWidget::CheckBtn btn);
private slots:
    void onToolButtonRelease();
private:
    Q_DECLARE_PRIVATE(SettingFactoryToolWidget)
    SettingFactoryToolWidgetPrivate* const d_ptr;
};

#endif // SETTINGFACTORYTOOLWIDGET_H
