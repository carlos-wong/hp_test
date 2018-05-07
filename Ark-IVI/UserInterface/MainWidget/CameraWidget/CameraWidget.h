#ifndef CAMERAWIDGET_H
#define CAMERAWIDGET_H

#include "BusinessLogic/Widget.h"
#include "BusinessLogic/Setting.h"
#include <QWidget>

class CameraWidgetPrivate;
class CameraWidget
        : private QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(CameraWidget)
public:
    explicit CameraWidget(QWidget *parent = NULL);
    ~CameraWidget();
protected:
    void resizeEvent(QResizeEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void paintEvent(QPaintEvent* event);
protected slots:
    void onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status);
    void onAVStatus(const int type, const int status);
private slots:
    void onTimeout();
private:
    Q_DECLARE_PRIVATE(CameraWidget)
    CameraWidgetPrivate* const d_ptr;
};

#endif // CAMERAWIDGET_H
