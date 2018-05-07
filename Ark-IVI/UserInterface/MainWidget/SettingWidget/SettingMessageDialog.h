#ifndef SETTINGMESSAGEDIALOG_H
#define SETTINGMESSAGEDIALOG_H

#include <QWidget>

class SettingMessageDialogPrivate;
class SettingMessageDialog : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(SettingMessageDialog)
public:
    explicit SettingMessageDialog(QWidget *parent = NULL);
    ~SettingMessageDialog();
    void setBtnEnable(const bool flag);
    void setBtnVisible(const bool flag);
    void setTitleTranslate(const bool flag);
    void setTitle(const QString& title);
    void setConfirm(const QString& confirm);
    void setCancel(const QString& cancel);
protected:
    void resizeEvent(QResizeEvent* event);
signals:
    void confirm();
    void cancel();
private:
    Q_DECLARE_PRIVATE(SettingMessageDialog)
    SettingMessageDialogPrivate* d_ptr;
};

#endif // SETTINGMESSAGEDIALOG_H
