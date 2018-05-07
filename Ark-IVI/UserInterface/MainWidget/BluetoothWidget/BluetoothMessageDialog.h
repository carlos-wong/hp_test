#ifndef BLUETOOTHMESSAGEDIALOG_H
#define BLUETOOTHMESSAGEDIALOG_H

#include <QWidget>

class BluetoothMessageDialogPrivate;
class BluetoothMessageDialog : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(BluetoothMessageDialog)
public:
    explicit BluetoothMessageDialog(QWidget *parent = NULL);
    ~BluetoothMessageDialog();
    void setConfirmVisible(const bool flag);
    void setTitle(const QString& title);
    QString getTitle();
    void setConfirm(const QString& confirm);
    void setCancel(const QString& cancel);
protected:
    void resizeEvent(QResizeEvent* event);
signals:
    void confirm();
    void cancel();
private:
    Q_DECLARE_PRIVATE(BluetoothMessageDialog)
    BluetoothMessageDialogPrivate* d_ptr;
};

#endif // BLUETOOTHMESSAGEDIALOG_H
