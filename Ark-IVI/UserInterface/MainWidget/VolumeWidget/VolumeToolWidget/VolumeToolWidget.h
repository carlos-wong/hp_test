#ifndef VOLUMETOOLWIDGET_H
#define VOLUMETOOLWIDGET_H

#include "BusinessLogic/Audio.h"
#include <QWidget>
#include <QScopedPointer>

class VolumeToolWidgetPrivate;
class VolumeToolWidget
        : protected QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(VolumeToolWidget)
public:
    explicit VolumeToolWidget(QWidget* parent = NULL);
    ~VolumeToolWidget();
protected:
    void resizeEvent(QResizeEvent* event);
    void changeEvent(QEvent* event);
protected slots:
    void onVolumeChange(const int volume);
private slots:
    void onMinusBtnRelease();
    void onPlusBtnRelease();
    void onTickMarksMillesimalEnd(const int millesimal);
private:
    friend class VolumeToolWidgetPrivate;
    QScopedPointer<VolumeToolWidgetPrivate> m_Private;
};

#endif // VOLUMETOOLWIDGET_H
