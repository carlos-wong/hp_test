#ifndef SETTINGSOUNDTOOLWIDGET_H
#define SETTINGSOUNDTOOLWIDGET_H

#include <QWidget>

class SettingSoundToolWidgetPrivate;
class SettingSoundToolWidget
        : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(SettingSoundToolWidget)
public:
    enum CheckBtn {
        CB_Equalizer = 0,
        CB_Sound,
    };
    explicit SettingSoundToolWidget(QWidget *parent = NULL);
    ~SettingSoundToolWidget();
    void setCheckBtn(const SettingSoundToolWidget::CheckBtn btn);
protected:
    void resizeEvent(QResizeEvent* event);
signals:
    void toolButtonRelease(const SettingSoundToolWidget::CheckBtn btn);
private slots:
    void onToolButtonRelease();
private:
    Q_DECLARE_PRIVATE(SettingSoundToolWidget)
    SettingSoundToolWidgetPrivate* const d_ptr;
};

#endif // SETTINGSOUNDTOOLWIDGET_H
