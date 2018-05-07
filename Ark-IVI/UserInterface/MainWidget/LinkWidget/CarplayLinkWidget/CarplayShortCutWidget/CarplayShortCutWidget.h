#ifndef CarplayShortCutWidget_H
#define CarplayShortCutWidget_H

#include "BmpButton.h"
#include "Link.h"
#include <QScopedPointer>

class CarplayShortCutWidgetPrivate;
class CarplayShortCutWidget
        : protected QWidget
        , protected Link::Interface
{
    Q_OBJECT
    Q_DISABLE_COPY(CarplayShortCutWidget)
public:
    explicit CarplayShortCutWidget(QWidget *parent = NULL);
    ~CarplayShortCutWidget();
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
    friend class CarplayShortCutWidgetPrivate;
    QScopedPointer<CarplayShortCutWidgetPrivate> m_Private;
};

#endif // CarplayShortCutWidget_H
