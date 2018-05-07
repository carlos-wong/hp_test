#ifndef SDDISKMUSICLISTVIEWWIDGET_H
#define SDDISKMUSICLISTVIEWWIDGET_H

#include "BusinessLogic/Widget.h"
#include "BusinessLogic/Multimedia.h"
#include <QWidget>
#include <QScopedPointer>

class SDDiskMusicListViewWidgetPrivate;
class SDDiskMusicListViewWidget
        : private QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(SDDiskMusicListViewWidget)
public:
    explicit SDDiskMusicListViewWidget(QWidget *parent = 0);
    ~SDDiskMusicListViewWidget();
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
    friend class SDDiskMusicListViewWidgetPrivate;
    QScopedPointer<SDDiskMusicListViewWidgetPrivate> m_Private;
};

#endif // SDDISKMUSICLISTVIEWWIDGET_H
