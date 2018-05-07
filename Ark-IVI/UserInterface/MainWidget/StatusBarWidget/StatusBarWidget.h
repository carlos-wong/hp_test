#ifndef STATUSBARWIDGET_H
#define STATUSBARWIDGET_H

#include "BusinessLogic/Multimedia.h"
#include "BusinessLogic/Bluetooth.h"
#include "BusinessLogic/Widget.h"
#include <QDateTime>
#include <QWidget>

class StatusBarWidgetPrivate;
class StatusBarWidget
        : private QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(StatusBarWidget)
public:
    explicit StatusBarWidget(QWidget *parent = NULL);
    ~StatusBarWidget();
protected:
    void resizeEvent(QResizeEvent* event);
protected slots:
    void onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status);
    void onDeviceWatcherStatus(const int type, const int status);
    void onConnectStatusChange(const BluetoothConnectStatus status);
    void onCurrentDateTime(const QDateTime &dateTime);
    void onHourClockChange(const int type);
private slots:
    void onBmpButtonRelease();
private:
    StatusBarWidgetPrivate* const d_ptr;
    Q_DECLARE_PRIVATE(StatusBarWidget)
};

#endif // STATUSBARWIDGET_H
