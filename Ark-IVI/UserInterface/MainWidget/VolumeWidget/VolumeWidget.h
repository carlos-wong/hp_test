#ifndef VOLUMEWIDGET_H
#define VOLUMEWIDGET_H

#include "BusinessLogic/Audio.h"
#include "BusinessLogic/Setting.h"
#include "BusinessLogic/Widget.h"
#include <QWidget>
#include <QScopedPointer>

class VolumeWidgetPrivate;
class VolumeWidget
        : private QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(VolumeWidget)
public:
    explicit VolumeWidget(QWidget* parent = NULL);
    ~VolumeWidget();
protected:
    void paintEvent(QPaintEvent* event);
    void resizeEvent(QResizeEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
protected slots:
    void onArkProtocol(const ArkProtocolWrapper& protocol);
    void onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status);
    void onMuteChange(const int mute);
    void onVolumeChange(const int volume);
private slots:
    void onTimeout();
private:
    friend class VolumeWidgetPrivate;
    QScopedPointer<VolumeWidgetPrivate> m_Private;
};

#endif // VOLUMEWIDGET_H
