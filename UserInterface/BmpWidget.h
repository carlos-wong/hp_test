#ifndef BMPWIDGET_H
#define BMPWIDGET_H

#include <QWidget>
#include <QScopedPointer>

class BmpWidgetPrivate;
class BmpWidget : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(BmpWidget)
public:
    explicit BmpWidget(QWidget *parent = NULL);
    ~BmpWidget();
    void setBackgroundBmpPath(const QString &path);
    void setImageAutoFillBackground(bool flag);
    void setWidgetAdaptToImage(bool flag);
protected:
    void paintEvent(QPaintEvent* event);
    void resizeEvent(QResizeEvent *);
private:
    QScopedPointer<BmpWidgetPrivate> m_Private;
};

#endif // BMPWIDGET_H
