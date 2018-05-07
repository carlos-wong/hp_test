#ifndef SETTINGGENERALTIMEWIDGET_H
#define SETTINGGENERALTIMEWIDGET_H

#include <QWidget>
#include <QDateTime>

class SettingGeneralTimeWidgetPrivate;
class SettingGeneralTimeWidget
        : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(SettingGeneralTimeWidget)
public:
    explicit SettingGeneralTimeWidget(QWidget* parent = NULL);
    ~SettingGeneralTimeWidget();
protected:
    void resizeEvent(QResizeEvent* event);
protected slots:
    void onHourClockChange(const int type);
private slots:
    void onCurrentDateTime(const QDateTime &dateTime);
    void onIndexChange(const int index);
private slots:
    void onBmpButtonRelease();
private:
    Q_DECLARE_PRIVATE(SettingGeneralTimeWidget)
    SettingGeneralTimeWidgetPrivate* const d_ptr;
};

#endif // SETTINGGENERALTIMEWIDGET_H
