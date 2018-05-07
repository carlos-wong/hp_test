#ifndef MUSICWIDGET_H
#define MUSICWIDGET_H

#include "BusinessLogic/Audio.h"
#include "BusinessLogic/Multimedia.h"
#include "BusinessLogic/Widget.h"
#include "BusinessLogic/Setting.h"
#include <QWidget>
#include <QScopedPointer>

class MusicWidgetPrivate;
class MusicWidget
        : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(MusicWidget)
public:
    explicit MusicWidget(QWidget *parent = NULL);
    ~MusicWidget();
protected:
    void resizeEvent(QResizeEvent *event);
protected slots:
    void onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status);
    void onDeviceWatcherStatus(const int type, const int status);
    void onMusicPlayerFileNames(const int type, const QString &xml);
    void onMusicPlayerID3TagChange(const int type,
                                   const int index,
                                   const QString &fileName,
                                   const QString& title,
                                   const QString& artist,
                                   const QString& album,
                                   const int endTime);
    void onMusicPlayerElapsedInformation(const int elapsedTime, const int elapsedMillesimal);
    void onMusicPlayerPlayStatus(const int type, const int status);
    void onArkProtocol(const ArkProtocolWrapper& protocol);
    void onHolderChange(const int oldHolder, const int newHolder);
private:
    friend class MusicWidgetPrivate;
    QScopedPointer<MusicWidgetPrivate> m_Private;
};

#endif // MUSICWIDGET_H
