#ifndef MUSICTOOLWIDGET_H
#define MUSICTOOLWIDGET_H

#include "BusinessLogic/Multimedia.h"
#include "BusinessLogic/Widget.h"
#include <QWidget>
#include <QScopedPointer>

class MusicToolWidgetPrivate;
class MusicToolWidget
        : private QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(MusicToolWidget)
public:
    explicit MusicToolWidget(QWidget *parent = NULL);
    ~MusicToolWidget();
protected:
    void resizeEvent(QResizeEvent *event);
protected slots:
    void onMusicPlayerPlayMode(const int mode);
    void onMusicPlayerPlayStatus(const int type, const int status);
public slots:
    void onToolButtonRelease();
private:
    friend class MusicToolWidgetPrivate;
    QScopedPointer<MusicToolWidgetPrivate> m_Private;
};

#endif // MUSICTOOLWIDGET_H
