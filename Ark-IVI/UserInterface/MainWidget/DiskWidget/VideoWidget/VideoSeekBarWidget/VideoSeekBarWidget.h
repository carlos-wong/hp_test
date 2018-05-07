#ifndef VIDEOSEEKBARWIDGET_H
#define VIDEOSEEKBARWIDGET_H

#include "BusinessLogic/Widget.h"
#include "BusinessLogic/Multimedia.h"
#include <QWidget>

class VideoSeekBarWidgetPrivate;
class VideoSeekBarWidget
        : private QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(VideoSeekBarWidget)
public:
    explicit VideoSeekBarWidget(QWidget *parent = NULL);
    ~VideoSeekBarWidget();
protected:
    void resizeEvent(QResizeEvent* event);
protected slots:
    void onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status);
    void onVideoPlayerInformation(const int type, const int index, const QString &fileName, const int endTime);
    void onVideoPlayerElapsedInformation(const int elapsedTime, const int elapsedMillesimal);
private slots:
    void onSliderReleased(const int value);
private:
    Q_DECLARE_PRIVATE(VideoSeekBarWidget)
    VideoSeekBarWidgetPrivate* const d_ptr;
};

#endif // VIDEOSEEKBARWIDGET_H
