#ifndef CALIBRATEWIDGET_H
#define CALIBRATEWIDGET_H

#include <QWidget>
#include <QProcess>

class CalibrateWidgetPrivate;
class CalibrateWidget : private QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(CalibrateWidget)
public:
    explicit CalibrateWidget(QWidget *parent = NULL);
    ~CalibrateWidget();
    void startCalibrate();
    void stopCalibrate();
protected:
    void paintEvent(QPaintEvent* event);
    void resizeEvent(QResizeEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
private slots:
    void onStarted();
    void onFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onError(QProcess::ProcessError error);
    void onStateChanged(QProcess::ProcessState state);
    void onReadyReadStandardOutput();
    void onReadyReadStandardError();
private:
    friend class Launcher;
    friend class LauncherPrivate;
    CalibrateWidgetPrivate* const d_ptr;
    Q_DECLARE_PRIVATE(CalibrateWidget)
};

#endif // CALIBRATEWIDGET_H
