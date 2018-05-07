#ifndef BLUETOOTHMUSICWIDGET_H
#define BLUETOOTHMUSICWIDGET_H

#include "BusinessLogic/Bluetooth.h"
#include "BusinessLogic/Widget.h"
#include <QWidget>

class BluetoothMusicWidgetPrivate;
class BluetoothMusicWidget
        : private QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(BluetoothMusicWidget)
public:
    explicit BluetoothMusicWidget(QWidget *parent = NULL);
    ~BluetoothMusicWidget();
protected:
    void resizeEvent(QResizeEvent* event);
protected slots:
    void onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status);
    void onMusicStatusChange(const QString& musicName, const BluetoothPlayerStatus status);
    void onConnectStatusChange(const BluetoothConnectStatus status);
private slots:
    void onToolButtonRelease();
private:
    Q_DECLARE_PRIVATE(BluetoothMusicWidget)
    BluetoothMusicWidgetPrivate* const d_ptr;
};

#endif // BLUETOOTHMUSICWIDGET_H
