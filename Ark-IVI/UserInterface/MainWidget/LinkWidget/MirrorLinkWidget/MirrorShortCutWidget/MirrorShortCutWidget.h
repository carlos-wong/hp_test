#ifndef MIRRORSHORTCUTWIDGET_H
#define MIRRORSHORTCUTWIDGET_H

#include "BmpButton.h"
#include "Link.h"
#include <QScopedPointer>

class MirrorShortCutWidgetPrivate;
class MirrorShortCutWidget
        : protected QWidget
        , protected Link::Interface
{
    Q_OBJECT
    Q_DISABLE_COPY(MirrorShortCutWidget)
public:
    explicit MirrorShortCutWidget(QWidget *parent = NULL);
    ~MirrorShortCutWidget();
protected:
    void resizeEvent(QResizeEvent* event) final;
    void customEvent(QEvent* event) final;
protected slots:
    //Link::Interface
    void onLinkStatusChange(const int type, const int status) final;
    //  void onLinkStatusChange(const Link_Type type, const Link_STATUS status);
private slots:
    void onBmpButtonRelease();
private:
    friend class MirrorShortCutWidgetPrivate;
    QScopedPointer<MirrorShortCutWidgetPrivate> m_Private;
};

#endif // MIRRORSHORTCUTWIDGET_H
