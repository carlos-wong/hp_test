#ifndef MIRRORMESSAGEDIALOG_H
#define MIRRORMESSAGEDIALOG_H

#include "BmpButton.h"
#include "BusinessLogic/Link.h"
#include <QWidget>
#include <QScopedPointer>

class MirrorMessageDialogPrivate;
class MirrorMessageDialog : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(MirrorMessageDialog)
public:
    explicit MirrorMessageDialog(QWidget *parent = NULL);
    ~MirrorMessageDialog();
    void setDeviceName(const QString& name);
    void setDeviceCode(const QString& code);
    void setDeviceStatus(const int flag);
protected:
    void resizeEvent(QResizeEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    bool event(QEvent* event);
private:
    friend class MirrorMessageDialogPrivate;
    QScopedPointer<MirrorMessageDialogPrivate> m_Private;
};

#endif // MIRRORMESSAGEDIALOG_H
