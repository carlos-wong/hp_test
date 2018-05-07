#ifndef USBDISKMUSICLISTVIEWWIDGET_H
#define USBDISKMUSICLISTVIEWWIDGET_H

#include "BusinessLogic/Widget.h"
#include "BusinessLogic/Multimedia.h"
#include <QWidget>
#include <QScopedPointer>

class USBDiskMusicListViewWidgetPrivate;
class USBDiskMusicListViewWidget
        : private QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(USBDiskMusicListViewWidget)
public:
    explicit USBDiskMusicListViewWidget(QWidget *parent = 0);
    ~USBDiskMusicListViewWidget();
protected:
    void resizeEvent(QResizeEvent* event);
protected slots:
    void onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status);
    void onDeviceWatcherStatus(const int type, const int status);
    void onMusicPlayerFileNames(const int type, const QString &xml);
    void onMusicPlayerPlayStatus(const int type, const int status);
    void onMusicPlayerID3TagChange(const int type,
                                   const int index,
                                   const QString &fileName,
                                   const QString& title,
                                   const QString& artist,
                                   const QString& album,
                                   const int endTime);
    void onMusicPlayerElapsedInformation(const int elapsedTime,
                                         const int elapsedMillesimal);
    void onImagePlayerChange(const int type, const QString &filePath, const int index, const int percent, const int rotate);
    void onVideoPlayerPlayStatus(const int type, const int status);
private slots:
    void onMusicListViewItemRelease(const int index);
private:
    friend class USBDiskMusicListViewWidgetPrivate;
    QScopedPointer<USBDiskMusicListViewWidgetPrivate> m_Private;
};

#endif // USBDISKMUSICLISTVIEWWIDGET_H
