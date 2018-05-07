#ifndef SETTINGGENERALVOLUMEWIDGET_H
#define SETTINGGENERALVOLUMEWIDGET_H

#include "BusinessLogic/Audio.h"
#include <QWidget>

class SettingGeneralVolumeWidgetPrivate;
class SettingGeneralVolumeWidget
        : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(SettingGeneralVolumeWidget)
public:
    explicit SettingGeneralVolumeWidget(QWidget* parent = NULL);
    ~SettingGeneralVolumeWidget();
protected:
    void resizeEvent(QResizeEvent* event);
protected slots:
    void onVolumeChange(const int volume);
private slots:
    void onTimeout();
    void onSliderMoved(const int value);
    void onSliderReleased(const int value);
private:
    Q_DECLARE_PRIVATE(SettingGeneralVolumeWidget)
    SettingGeneralVolumeWidgetPrivate* const d_ptr;
};

#endif // SETTINGGENERALVOLUMEWIDGET_H
