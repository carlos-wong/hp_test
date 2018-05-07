#ifndef IMAGEWIDGET_H
#define IMAGEWIDGET_H

#include "BusinessLogic/Multimedia.h"
#include "BusinessLogic/Widget.h"
#include "BusinessLogic/Setting.h"
#include <QWidget>
#include <QImage>
#include <QScopedPointer>

class ImageWidgetPrivate;
class ImageWidget
        : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(ImageWidget)
public:
    explicit ImageWidget(QWidget *parent = NULL);
    ~ImageWidget();
protected:
    void paintEvent(QPaintEvent* event);
    void resizeEvent(QResizeEvent *event);
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
protected slots:
    void onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status);
    void onDeviceWatcherStatus(const int type, const int status);
    void onImagePlayerFileNames(const int type, const QString &xml);
    void onImagePlayerChange(const int type, const QString &filePath, const int index, const int percent, const int rotate);
    void onArkProtocol(const ArkProtocolWrapper& protocol);
private slots:
    void onStartTimer();
    void onStopTimer();
signals:
    void startLoading();
    void imageChange();
private slots:
    void onImageChange();
    void onStartLoading();
    void onTimeout();
    void onToolButtonRelease(const int type);
private:
    friend class ImageWidgetPrivate;
    QScopedPointer<ImageWidgetPrivate> m_Private;
};

#endif // IMAGEWIDGET_H
