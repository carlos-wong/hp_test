#ifndef SETTINGSTYLEWALLPAPERWIDGET_H
#define SETTINGSTYLEWALLPAPERWIDGET_H

#include "BusinessLogic/Widget.h"
#include <QWidget>

class SettingStyleWallpaperWidgetPrivate;
class SettingStyleWallpaperWidget
        : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(SettingStyleWallpaperWidget)
public:
    explicit SettingStyleWallpaperWidget(QWidget *parent = NULL);
    ~SettingStyleWallpaperWidget();
protected:
    void resizeEvent(QResizeEvent* event);
protected slots:
    void onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status);
    void onThemeChange(const int type);
private slots:
    void onToolButtonRelease();
private:
    Q_DECLARE_PRIVATE(SettingStyleWallpaperWidget)
    SettingStyleWallpaperWidgetPrivate* const d_ptr;
};

#endif // SETTINGSTYLEWALLPAPERWIDGET_H
