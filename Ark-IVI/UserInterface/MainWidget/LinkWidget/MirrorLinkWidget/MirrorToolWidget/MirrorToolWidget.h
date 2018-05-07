#ifndef MIRRORTOOLWIDGET_H
#define MIRRORTOOLWIDGET_H

#include <QWidget>
#include <QScopedPointer>

class MirrorToolWidgetPrivate;
class MirrorToolWidget : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(MirrorToolWidget)
public:
    enum Type {
        T_BlueTooth = 0,
        T_Menu = 1,
        T_Car = 2,
        T_Home = 3,
        T_Back = 4,
    };
    explicit MirrorToolWidget(QWidget *parent = NULL);
    ~MirrorToolWidget();
protected:
    void resizeEvent(QResizeEvent* event);
    void timerEvent(QTimerEvent* event);
    bool event(QEvent* event);
signals:
    void toolBtnRelease(const int type);
private slots:
    void onToolBtnRelease();
private:
    friend class MirrorToolWidgetPrivate;
    QScopedPointer<MirrorToolWidgetPrivate> m_Private;
};

#endif // MIRRORTOOLWIDGET_H
