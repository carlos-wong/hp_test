#ifndef SDDISKTOOLWIDGET_H
#define SDDISKTOOLWIDGET_H

#include "BusinessLogic/Widget.h"
#include "BusinessLogic/Multimedia.h"
#include <QWidget>
#include <QScopedPointer>

class SDDiskToolWidgetPrivate;
class SDDiskToolWidget
        : protected QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(SDDiskToolWidget)
public:
    explicit SDDiskToolWidget(QWidget *parent = NULL);
    ~SDDiskToolWidget();
protected:
    void resizeEvent(QResizeEvent* event);
protected slots:
    void onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status);
    void onDeviceWatcherStatus(const int type, const int status);
    //        void onDeviceWatcherStatus(const DeviceWatcherType type, const DeviceWatcherStatus status);
private slots:
    void onToolButtonRelease();
private:
    friend class SDDiskToolWidgetPrivate;
    QScopedPointer<SDDiskToolWidgetPrivate> m_Private;
};

#endif // SDDISKTOOLWIDGET_H
