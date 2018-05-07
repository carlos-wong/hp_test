#ifndef SETTINGGENERALTOOLWIDGET_H
#define SETTINGGENERALTOOLWIDGET_H

#include <QWidget>

class SettingGeneralToolWidgetPrivate;
class SettingGeneralToolWidget
        : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(SettingGeneralToolWidget)
public:
    enum CheckBtn {
        CB_Dispaly = 0,
        CB_Volume = 1,
        CB_Language = 2,
        CB_Time = 3,
        CB_Version = 4,
    };
    explicit SettingGeneralToolWidget(QWidget* parent = NULL);
    ~SettingGeneralToolWidget();
    void setCheckBtn(const SettingGeneralToolWidget::CheckBtn btn);
protected:
    void resizeEvent(QResizeEvent* event);
signals:
    void toolButtonRelease(const SettingGeneralToolWidget::CheckBtn btn);
private slots:
    void onToolButtonRelease();
private:
    Q_DECLARE_PRIVATE(SettingGeneralToolWidget)
    SettingGeneralToolWidgetPrivate* const d_ptr;
};

#endif // SETTINGGENERALTOOLWIDGET_H
