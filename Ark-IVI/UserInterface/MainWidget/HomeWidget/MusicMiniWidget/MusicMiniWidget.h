#ifndef MUSICMINIWIDGET_H
#define MUSICMINIWIDGET_H

#include "BusinessLogic/Widget.h"
#include "BusinessLogic/Multimedia.h"
#include <QWidget>

class MusicMiniWidgetPrivate;
class MusicMiniWidget
        : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(MusicMiniWidget)
public:
    explicit MusicMiniWidget(QWidget *parent = NULL);
    ~MusicMiniWidget();
protected:
    void resizeEvent(QResizeEvent* event);
    void paintEvent(QPaintEvent *);
protected slots:
    void onMusicPlayerPlayStatus(const int type, const int status);
    void onMusicPlayerID3TagChange(const int type, const int index, const QString &fileName, const QString& title,
                                   const QString& artist, const QString& album, const int endTime);
private slots:
    void onBmpButtonRelease();
private:
    Q_DECLARE_PRIVATE(MusicMiniWidget)
    MusicMiniWidgetPrivate* const d_ptr;
};

#endif // MUSICMINIWIDGET_H
