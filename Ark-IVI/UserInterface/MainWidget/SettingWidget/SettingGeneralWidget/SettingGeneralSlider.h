#ifndef SETTINGGENERALSLIDER_H
#define SETTINGGENERALSLIDER_H

#include <QWidget>

class SettingGeneralSliderPrivate;
class SettingGeneralSlider
        : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(SettingGeneralSlider)
public:
    explicit SettingGeneralSlider(QWidget* parent = NULL);
    ~SettingGeneralSlider();
    void setText(const QString& text);
    void setRange(const int minimum, const int maximum);
    void setValue(const int value);
protected:
    void resizeEvent(QResizeEvent* event);
signals:
    void sliderReleased(const int value);
private:
    Q_DECLARE_PRIVATE(SettingGeneralSlider)
    SettingGeneralSliderPrivate* const d_ptr;
};

#endif // SETTINGGENERALSLIDER_H
