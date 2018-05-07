#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include "VideoToolBarWidget/VideoToolBarWidget.h"
#include "BusinessLogic/Multimedia.h"
#include "BusinessLogic/Widget.h"
#include "BusinessLogic/Setting.h"
#include <QWidget>
#include <QScopedPointer>

class VideoWidgetPrivate;
class VideoWidget
        : private QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(VideoWidget)
public:
    explicit VideoWidget(QWidget *parent = NULL);
    ~VideoWidget();
protected:
    void resizeEvent(QResizeEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
protected slots:
    void onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status);
    void onDeviceWatcherStatus(const int type, const int status);
    void onVideoPlayerFileNames(const int type, const QString& xml);
    void onVideoPlayerPlayStatus(const int type, const int status);
    void onVideoPlayerInformation(const int type, const int index, const QString &fileName, const int endTime);
    void onVideoPlayerElapsedInformation(const int elapsedTime, const int elapsedMillesimal);
    void onArkProtocol(const ArkProtocolWrapper& protocol);
    void onHolderChange(const int oldHolder, const int newHolder);
private slots:
    void onTimeout();
private:
    friend class VideoWidgetPrivate;
    QScopedPointer<VideoWidgetPrivate> m_Private;
};

#endif // VIDEOWIDGET_H
