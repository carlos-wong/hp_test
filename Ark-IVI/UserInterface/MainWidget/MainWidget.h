#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "BusinessLogic/Widget.h"
#include <QWidget>
#include <QScopedPointer>

class MainWidgetPrivate;
class MainWidget
        : private QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(MainWidget)
public:
    explicit MainWidget(QWidget* parent = NULL);
    ~MainWidget();
    void makeSureComplete();
protected:
    void resizeEvent(QResizeEvent* event);
protected slots:
    void onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status);
    void onThemeChange(const int type);
private slots:
    void onStartComplete();
    void onActivated();
    void onReveringWidgetVisibleChanged(bool visible);
private:
    friend class Launcher;
    friend class LauncherPrivate;
    friend class MainWidgetPrivate;
    QScopedPointer<MainWidgetPrivate> m_Private;
};

#endif // MAINWINDOW_H
