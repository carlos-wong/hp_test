#ifndef SETTINGFACTORYNUMBERPADWIDGET_H
#define SETTINGFACTORYNUMBERPADWIDGET_H

#include <QWidget>

class SettingFactoryNumberPadWidgetPrivate;
class SettingFactoryNumberPadWidget
        : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(SettingFactoryNumberPadWidget)
public:
    explicit SettingFactoryNumberPadWidget(QWidget* parent = NULL);
    ~SettingFactoryNumberPadWidget();
    void setPassword(const QString& pincode);
signals:
    void numberPadKeyType(const QString& type);
protected:
    void resizeEvent(QResizeEvent* event);
    void paintEvent(QPaintEvent* event);
private slots:
    void onReleased();
private:
    Q_DECLARE_PRIVATE(SettingFactoryNumberPadWidget)
    SettingFactoryNumberPadWidgetPrivate* const d_ptr;
};

#endif // SETTINGFACTORYNUMBERPADWIDGET_H
