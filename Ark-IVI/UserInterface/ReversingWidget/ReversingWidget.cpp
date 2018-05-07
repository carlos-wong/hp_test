#include "ReversingWidget.h"
#include "AutoConnect.h"
#include "BmpWidget.h"
#include "TextWidget.h"
#include "MessageBox.h"
#include <QDebug>
#include <QFile>

class ReversingWidgetPrivate {
public:
    explicit ReversingWidgetPrivate(ReversingWidget* parent);
    ~ReversingWidgetPrivate();
    void initializeParent();
private:
    Q_DECLARE_PUBLIC(ReversingWidget)
    ReversingWidget* const q_ptr;
};

ReversingWidget::ReversingWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new ReversingWidgetPrivate(this))
{
}

ReversingWidget::~ReversingWidget()
{
    if (NULL != d_ptr) {
        delete d_ptr;
    }
}

void ReversingWidget::onArkCarHandler(const ArkCar *car)
{
    if (NULL != car) {
        if (ACS_ReversingOn == car->status) {
            raise();
            setVisible(true);
        } else if (ACS_ReversingOff == car->status) {
            setVisible(false);
        }
    }
}

void ReversingWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(0, 0, 800, 480, this);
}

ReversingWidgetPrivate::ReversingWidgetPrivate(ReversingWidget *parent)
    : q_ptr(parent)
{
    initializeParent();
}

ReversingWidgetPrivate::~ReversingWidgetPrivate()
{
}

void ReversingWidgetPrivate::initializeParent()
{
    Q_Q(ReversingWidget);
    q->setVisible(false);
}
