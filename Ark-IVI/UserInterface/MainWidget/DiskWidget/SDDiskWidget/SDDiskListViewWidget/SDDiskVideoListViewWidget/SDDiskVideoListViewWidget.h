#ifndef SDDISKVIDEOLISTVIEWWIDGET_H
#define SDDISKVIDEOLISTVIEWWIDGET_H

#include "BusinessLogic/Widget.h"
#include "BusinessLogic/Multimedia.h"
#include <QWidget>
#include <QScopedPointer>

class SDDiskVideoListViewWidgetPrivate;
class SDDiskVideoListViewWidget
        : protected QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(SDDiskVideoListViewWidget)
public:
    explicit SDDiskVideoListViewWidget(QWidget *parent = 0);
    ~SDDiskVideoListViewWidget();
protected:
    void resizeEvent(QResizeEvent* event);
protected slots:
    void onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status);
    void onDeviceWatcherStatus(const int type, const int status);
    void onVideoPlayerFileNames(const int type, const QString& xml);
    void onVideoPlayerPlayStatus(const int type, const int status);
    void onVideoPlayerInformation(const int type, const int index, const QString &fileName, const int endTime);
    void onVideoPlayerElapsedInformation(const int elapsedTime, const int elapsedMillesimal);
    void onImagePlayerChange(const int type, const QString &filePath, const int index, const int percent, const int rotate);
    void onMusicPlayerPlayStatus(const int type, const int status);
private slots:
    void onVideoListViewItemRelease(const int index);
private:
    friend class SDDiskVideoListViewWidgetPrivate;
    QScopedPointer<SDDiskVideoListViewWidgetPrivate> m_Private;
};

#endif // SDDISKVIDEOLISTVIEWWIDGET_H
