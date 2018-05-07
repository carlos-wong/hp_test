#ifndef PIXMAPWIDGET_H
#define PIXMAPWIDGET_H

#include <QWidget>
#include <QScopedPointer>

class PixmapWidgetPrivate;
class PixmapWidget : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(PixmapWidget)
public:
    explicit PixmapWidget(QWidget *parent);
    ~PixmapWidget();
    void onImageChange(const QImage& image, const bool change = true, const bool gif = false);
    void rotate();
    void scale();
    void zoomOut();
    void zoomIn();
protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    bool event(QEvent* event);
signals:
    void stopTimer();
    void startTimer();
private:
    friend class PixmapWidgetPrivate;
    QScopedPointer<PixmapWidgetPrivate> m_Private;
};
#endif // PIXMAPWIDGET_H
