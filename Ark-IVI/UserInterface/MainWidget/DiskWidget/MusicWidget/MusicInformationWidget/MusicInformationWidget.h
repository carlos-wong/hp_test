#ifndef MUSICINFORMATIONWIDGET_H
#define MUSICINFORMATIONWIDGET_H

#include "BusinessLogic/Multimedia.h"
#include <QWidget>

class MusicInformationWidgetPrivate;
class MusicInformationWidget
        : private QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(MusicInformationWidget)
public:
    explicit MusicInformationWidget(QWidget *parent = NULL);
    ~MusicInformationWidget();
protected:
    void resizeEvent(QResizeEvent* event);
    void paintEvent(QPaintEvent* event);
protected slots:
    void onMusicPlayerID3TagChange(const int type,
                                   const int index,
                                   const QString &fileName,
                                   const QString& title,
                                   const QString& artist,
                                   const QString& album,
                                   const int endTime);
private:
    Q_DECLARE_PRIVATE(MusicInformationWidget)
    MusicInformationWidgetPrivate* const d_ptr;
};

#endif // MUSICINFORMATIONWIDGET_H
