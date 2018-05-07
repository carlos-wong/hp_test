#ifndef SETTINGSOUNDSOUNDWIDGET_H
#define SETTINGSOUNDSOUNDWIDGET_H

#include "BusinessLogic/Audio.h"
#include <QWidget>

class SettingSoundSoundWidgetPrivate;
class SettingSoundSoundWidget
        : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(SettingSoundSoundWidget)
public:
    explicit SettingSoundSoundWidget(QWidget *parent = NULL);
    ~SettingSoundSoundWidget();
protected:
    void resizeEvent(QResizeEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
protected slots:
    void onSoundItemChange(const int item, const int left, const int right);
private slots:
    void onTimeout();
    void onToolButtonRelease();
private:
    Q_DECLARE_PRIVATE(SettingSoundSoundWidget)
    SettingSoundSoundWidgetPrivate* const d_ptr;
};

#endif // SETTINGSOUNDSOUNDWIDGET_H
