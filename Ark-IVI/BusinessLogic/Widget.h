#ifndef WIDGET_H
#define WIDGET_H

#include "ArkApplication.h"
#include <QObject>
#include <QEvent>
#include <QRect>
#include <QScopedPointer>

namespace WidgetStatus {
static const QString RequestShow("RequestShow");
static const QString RequestHide("RequestHide");
static const QString Show("Show");
}

class WidgetPrivate;
class Widget : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(Widget)
#ifdef g_Widget
#undef g_Widget
#endif
#define g_Widget (Widget::instance())
public:
    enum Type {
        T_Undefine = -1,
        T_Back = 0,
        T_Home = 1,
        T_ViewPaper0 = 2,
        T_ViewPaper1 = 3,
        T_Radio = 4,
        T_Bluetooth = 5,
        T_BluetoothDial = 6,
        T_BluetoothContact = 7,
        T_BluetoothRecord = 8,
        T_BluetoothMusic = 9,
        T_BluetoothSetting = 10,
        T_SDDisk = 11,
        T_SDDiskMusic = 12,
        T_SDDiskImage = 13,
        T_SDDiskVideo = 14,
        T_SDMusic = 15,
        T_SDImage = 16,
        T_SDImageFullScreen = 17,
        T_SDVideo = 18,
        T_SDVideoFullScreen = 19,
        T_USBDisk = 20,
        T_USBDiskMusic = 21,
        T_USBDiskImage = 22,
        T_USBDiskVideo = 23,
        T_USBMusic = 24,
        T_USBImage = 25,
        T_USBImageFullScreen = 26,
        T_USBVideo = 27,
        T_USBVideoFullScreen = 28,
        T_Link = 29,
        T_Carlife = 30,
        T_Auto = 31,
        T_Carplay = 32,
        T_AUX = 33,
        T_AUXFullScreen = 34,
        T_Camera = 35,
        T_CameraFullScreen = 36,
        T_Steering = 37,
        T_Setting = 38,
        T_SettingGeneral = 39,
        T_SettingStyle = 40,
        T_SettingSound = 41,
        T_SettingFactory = 42,
    };
    void reset();
    QRect geometryFit(const int x, const int y, const int w, const int h, QWidget* widget = NULL);
    float widthScalabilityFactor();
    float heightScalabilityFactor();
    void acquireWidgetType(const Widget::Type type);
    void releaseWidgetType(const Widget::Type type);
    void setWidgetType(const Widget::Type destinationType, const Widget::Type requestType, const QString &status);
    const Widget::Type currentWidgetType() const;
    inline static Widget* instance() {
        static Widget* widget(new Widget(qApp));
        return widget;
    }
signals:
    void onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status);
private:
    explicit Widget(QObject* parent = NULL);
    ~Widget();
    void initializePrivate();
    friend class WidgetPrivate;
    QScopedPointer<WidgetPrivate> m_Private;
};

#endif // WIDGET_H
