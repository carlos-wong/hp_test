#ifndef USBDISKIMAGELISTVIEWWIDGET_H
#define USBDISKIMAGELISTVIEWWIDGET_H

#include "BusinessLogic/Widget.h"
#include "BusinessLogic/Multimedia.h"
#include <QWidget>
#include <QScopedPointer>

class USBDiskImageListViewWidgetPrivate;
class USBDiskImageListViewWidget
        : private QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(USBDiskImageListViewWidget)
public:
    explicit USBDiskImageListViewWidget(QWidget *parent = 0);
    ~USBDiskImageListViewWidget();
protected:
    void resizeEvent(QResizeEvent* event);
protected slots:
    void onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status);
    void onDeviceWatcherStatus(const int type, const int status);
    void onImagePlayerFileNames(const int type, const QString &xml);
    void onImagePlayerChange(const int type, const QString &filePath, const int index, const int percent, const int rotate);
    void onMusicPlayerPlayStatus(const int type, const int status);
    void onVideoPlayerPlayStatus(const int type, const int status);
public slots:
    void onImageListViewItemRelease(const int index);
private:
    friend class USBDiskImageListViewWidgetPrivate;
    friend void playUSBImageType(void);
    QScopedPointer<USBDiskImageListViewWidgetPrivate> m_Private;
};

#endif // USBDISKIMAGELISTVIEWWIDGET_H
