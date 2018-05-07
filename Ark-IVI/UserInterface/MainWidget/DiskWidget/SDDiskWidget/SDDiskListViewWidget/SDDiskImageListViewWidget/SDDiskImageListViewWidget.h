#ifndef SDDISKIMAGELISTVIEWWIDGET_H
#define SDDISKIMAGELISTVIEWWIDGET_H

#include "BusinessLogic/Widget.h"
#include "BusinessLogic/Multimedia.h"
#include <QWidget>
#include <QScopedPointer>

class SDDiskImageListViewWidgetPrivate;
class SDDiskImageListViewWidget
        : private QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(SDDiskImageListViewWidget)
public:
    explicit SDDiskImageListViewWidget(QWidget *parent = 0);
    ~SDDiskImageListViewWidget();
protected:
    void resizeEvent(QResizeEvent* event);
protected slots:
    void onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status);
    void onDeviceWatcherStatus(const int type, const int status);
    void onImagePlayerFileNames(const int type, const QString &xml);
    void onImagePlayerChange(const int type, const QString &filePath, const int index, const int percent, const int rotate);
    void onMusicPlayerPlayStatus(const int type, const int status);
    void onVideoPlayerPlayStatus(const int type, const int status);
private slots:
    void onImageListViewItemRelease(const int index);
private:
    friend class SDDiskImageListViewWidgetPrivate;
    QScopedPointer<SDDiskImageListViewWidgetPrivate> m_Private;
};

#endif // SDDISKIMAGELISTVIEWWIDGET_H
