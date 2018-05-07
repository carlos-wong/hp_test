#ifndef BRANDMINIWIDGET_H
#define BRANDMINIWIDGET_H

#include "BusinessLogic/Radio.h"
#include <QWidget>

class BrandMiniWidgetPrivate;
class BrandMiniWidget
        : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(BrandMiniWidget)
public:
    explicit BrandMiniWidget(QWidget *parent = NULL);
    ~BrandMiniWidget();
protected:
    void resizeEvent(QResizeEvent* event);
private:
    Q_DECLARE_PRIVATE(BrandMiniWidget)
    BrandMiniWidgetPrivate* const d_ptr;
};

#endif // BRANDMINIWIDGET_H
