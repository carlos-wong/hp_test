#ifndef MUSICSEEKBARWIDGET_H
#define MUSICSEEKBARWIDGET_H

#include "BusinessLogic/Multimedia.h"
#include <QWidget>

class MusicSeekBarWidgetPrivate;
class MusicSeekBarWidget
        : private QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(MusicSeekBarWidget)
public:
    explicit MusicSeekBarWidget(QWidget *parent = NULL);
    ~MusicSeekBarWidget();
protected:
    void resizeEvent(QResizeEvent* event);
protected slots:
    void onMusicPlayerElapsedInformation(const int elapsedTime, const int elapsedMillesimal);
    void onMusicPlayerID3TagChange(const int type,
                                   const int index,
                                   const QString &fileName,
                                   const QString& title,
                                   const QString& artist,
                                   const QString& album,
                                   const int endTime);
    void onMusicPlayerPlayStatus(const int type, const int status);
private slots:
    void onSliderReleased(const int value);
private:
    Q_DECLARE_PRIVATE(MusicSeekBarWidget)
    MusicSeekBarWidgetPrivate* const d_ptr;
};

#endif // MUSICSEEKBARWIDGET_H
