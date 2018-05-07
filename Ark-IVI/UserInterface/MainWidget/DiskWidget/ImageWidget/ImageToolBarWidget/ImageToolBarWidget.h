#ifndef IMAGETOOLBARWIDGET_H
#define IMAGETOOLBARWIDGET_H

#include "BusinessLogic/Widget.h"
#include "BusinessLogic/Multimedia.h"
#include <QWidget>
#include <QScopedPointer>

class ImageToolBarWidgetPrivate;
class ImageToolBarWidget
        : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(ImageToolBarWidget)
public:
    enum PlayStatus {
        PS_Pause,
        PS_Play,
    };
    enum ButtonType {
        BT_Rotate,
        BT_Previous,
        BT_Toggle,
        BT_Next,
        BT_ZoomIn,
        BT_ZoomOut,
    };
    explicit ImageToolBarWidget(QWidget *parent = NULL);
    ~ImageToolBarWidget();
    void setDisabledZoom(const bool flag);
    void setToolButtonStatus(const ImageToolBarWidget::PlayStatus status);
protected:
    void resizeEvent(QResizeEvent *event);
protected slots:
    void onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status) ;
private slots:
    void onToolButtonRelease();
signals:
    void toolButtonRelease(const int type);
private:
    friend class ImageToolBarWidgetPrivate;
    QScopedPointer<ImageToolBarWidgetPrivate> m_Private;
};

#endif // IMAGETOOLBARWIDGET_H
