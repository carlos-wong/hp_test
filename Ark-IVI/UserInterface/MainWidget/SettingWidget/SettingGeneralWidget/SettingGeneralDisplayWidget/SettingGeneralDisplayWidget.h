#ifndef SETTINGGENERALDISPLAYWIDGET_H
#define SETTINGGENERALDISPLAYWIDGET_H

#include <QWidget>

class SettingGeneralDisplayWidgetPrivate;
class SettingGeneralDisplayWidget
        : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(SettingGeneralDisplayWidget)
public:
    explicit SettingGeneralDisplayWidget(QWidget* parent = NULL);
    ~SettingGeneralDisplayWidget();
protected:
    void resizeEvent(QResizeEvent* event);
protected slots:
    void onBrightnessChange(const int value);
    void onContrastChange(const int value);
    void onSaturationChange(const int value);
private slots:
    void onSliderReleased(const int value);
private:
    Q_DECLARE_PRIVATE(SettingGeneralDisplayWidget)
    SettingGeneralDisplayWidgetPrivate* const d_ptr;
};

#endif // SETTINGGENERALDISPLAYWIDGET_H
