#ifndef BLUETOOTHSETTINGPAIREDLISTITEM_H
#define BLUETOOTHSETTINGPAIREDLISTITEM_H

#include <QWidget>

class BluetoothSettingPairedListItemPrivate;
class BluetoothSettingPairedListItem : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(BluetoothSettingPairedListItem)
public:
    explicit BluetoothSettingPairedListItem(QWidget *parent = NULL);
    ~BluetoothSettingPairedListItem();
    void setDeleteVisible(const bool flag);
    void setItemText(const QString& text);
    void setEnabled(bool flag);
protected:
    void resizeEvent(QResizeEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void paintEvent(QPaintEvent* event);
signals:
    void connectBtnRelease();
    void deleteBtnRelease();
private:
    Q_DECLARE_PRIVATE(BluetoothSettingPairedListItem)
    BluetoothSettingPairedListItemPrivate* d_ptr;
};

#endif // BLUETOOTHSETTINGPAIREDLISTITEM_H
