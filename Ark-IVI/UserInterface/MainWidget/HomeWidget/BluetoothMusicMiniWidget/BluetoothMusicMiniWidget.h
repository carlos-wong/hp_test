#ifndef BLUETOOTHMUSICMINIWIDGET_H
#define BLUETOOTHMUSICMINIWIDGET_H

#include "BusinessLogic/Widget.h"
#include "BusinessLogic/Bluetooth.h"
#include <QWidget>

class BluetoothMusicMiniWidgetPrivate;
class BluetoothMusicMiniWidget
        : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(BluetoothMusicMiniWidget)
public:
    explicit BluetoothMusicMiniWidget(QWidget *parent = NULL);
    ~BluetoothMusicMiniWidget();
protected:
    void resizeEvent(QResizeEvent* event);
    void paintEvent(QPaintEvent *);
protected slots:
    void onMusicStatusChange(const QString& musicName, const BluetoothPlayerStatus status);
private slots:
    void onBmpButtonRelease();
private:
    Q_DECLARE_PRIVATE(BluetoothMusicMiniWidget)
    BluetoothMusicMiniWidgetPrivate* const d_ptr;
};

#endif // BLUETOOTHMUSICMINIWIDGET_H
