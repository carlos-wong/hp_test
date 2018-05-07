#ifndef LAUNCHER_H
#define LAUNCHER_H

#include "BusinessLogic/Setting.h"
#include <QWidget>

class LauncherPrivate;
class Launcher : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(Launcher)
public:
    explicit Launcher(QWidget *parent = NULL);
    ~Launcher();
protected:
    void timerEvent(QTimerEvent* event);
    void resizeEvent(QResizeEvent* event);
protected slots:
    void onArkProtocol(const ArkProtocolWrapper& protocol);
private slots:
    void onStartComplete();
private:
    LauncherPrivate* const d_ptr;
    Q_DECLARE_PRIVATE(Launcher)
};

#endif // LAUNCHER_H
