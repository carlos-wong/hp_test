#ifndef BLUETOOTHSETTINGALPHABETPADWIDGET_H
#define BLUETOOTHSETTINGALPHABETPADWIDGET_H

#include <QWidget>

class BluetoothSettingAlphabetPadWidgetPrivate;
class BluetoothSettingAlphabetPadWidget
        : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(BluetoothSettingAlphabetPadWidget)
public:
    explicit BluetoothSettingAlphabetPadWidget(QWidget *parent = NULL);
    ~BluetoothSettingAlphabetPadWidget();
    void setDeviceName(const QString& name);
signals:
    void alphabetPadKeyType(const QString& type);
protected:
    void paintEvent(QPaintEvent* event);
    void resizeEvent(QResizeEvent* event);
private slots:
    void onReleased();
private:
    Q_DECLARE_PRIVATE(BluetoothSettingAlphabetPadWidget)
    BluetoothSettingAlphabetPadWidgetPrivate* const d_ptr;
};

#endif // BLUETOOTHSETTINGALPHABETPADWIDGET_H
