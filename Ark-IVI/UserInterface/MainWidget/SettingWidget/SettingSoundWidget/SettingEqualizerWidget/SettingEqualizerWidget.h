#ifndef SETTINGEQUALIZERWIDGET_H
#define SETTINGEQUALIZERWIDGET_H

#include "BusinessLogic/Audio.h"
#include <QWidget>

class SettingEqualizerWidgetPrivate;
class SettingEqualizerWidget
        : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(SettingEqualizerWidget)
public:
    explicit SettingEqualizerWidget(QWidget *parent = NULL);
    ~SettingEqualizerWidget();
protected:
    void resizeEvent(QResizeEvent* event);
private slots:
    void onEqualizerItemChange(const int item, const int bass, const int middle, const int treble);
    void onSliderMoved(const int value);
    void onSliderReleased(const int value);
    void onToolButtonRelease();
    void onTimeout();
private:
    Q_DECLARE_PRIVATE(SettingEqualizerWidget)
    SettingEqualizerWidgetPrivate* const d_ptr;
};

#endif // SETTINGEQUALIZERWIDGET_H
