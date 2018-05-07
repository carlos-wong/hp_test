#ifndef VIDEOTOOLBARWIDGET_H
#define VIDEOTOOLBARWIDGET_H

#include "BusinessLogic/Multimedia.h"
#include "BusinessLogic/Widget.h"
#include <QWidget>
#include <QScopedPointer>

class VideoToolBarWidgetPrivate;
class VideoToolBarWidget
        : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(VideoToolBarWidget)
public:
    explicit VideoToolBarWidget(QWidget *parent = NULL);
    ~VideoToolBarWidget();
protected:
    void resizeEvent(QResizeEvent* event);
protected slots:
    void onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status);
    void onVideoPlayerPlayStatus(const int type, const int status);
    void onVideoPlayerPlayMode(const int mode);
private slots:
    void onToolButtonRelease();
private:
    friend class VideoToolBarWidgetPrivate;
    QScopedPointer<VideoToolBarWidgetPrivate> m_Private;
};

#endif // VIDEOTOOLBARWIDGET_H
