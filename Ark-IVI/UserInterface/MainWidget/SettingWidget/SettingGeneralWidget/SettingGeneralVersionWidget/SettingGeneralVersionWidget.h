#ifndef SETTINGGENERALVERSIONWIDGET_H
#define SETTINGGENERALVERSIONWIDGET_H

#include "BusinessLogic/Multimedia.h"
#include "ArkProtocolWrapper.h"
#include <QWidget>

class SettingGeneralVersionWidgetPrivate;
class SettingGeneralVersionWidget : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(SettingGeneralVersionWidget)
public:
    explicit SettingGeneralVersionWidget(QWidget *parent = NULL);
    ~SettingGeneralVersionWidget();
protected:
    void resizeEvent(QResizeEvent* event);
protected slots:
    void onArkProtocol(const ArkProtocolWrapper& protocol);
    void onDeviceWatcherStatus(const int type, const int status);
private slots:
    void onBmpButtonRelease();
    void onConfirm();
    void onCancel();
private:
    Q_DECLARE_PRIVATE(SettingGeneralVersionWidget)
    SettingGeneralVersionWidgetPrivate* const d_ptr;
};

#endif // SETTINGGENERALVERSIONWIDGET_H
