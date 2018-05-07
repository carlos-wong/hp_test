#include "ViewPaperTipWidget.h"
#include "BusinessLogic/Widget.h"
#include "AutoConnect.h"
#include <QPainter>
#include <QPixmap>
#include <QDebug>

class ViewPaperTipWidgetPrivate
{
public:
    ViewPaperTipWidgetPrivate(ViewPaperTipWidget* parent);
    ~ViewPaperTipWidgetPrivate();
    void initializeParent();
    void initializeBasic();
private:
    Q_DECLARE_PUBLIC(ViewPaperTipWidget)
    ViewPaperTipWidget* const q_ptr;
    QPixmap* m_VisibleTip;
    QPixmap* m_InvisibleTip;
    QPixmap* m_Viewpaper0;
    QPixmap* m_ViewPaper1;
};

ViewPaperTipWidget::ViewPaperTipWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new ViewPaperTipWidgetPrivate(this))
{
}

ViewPaperTipWidget::~ViewPaperTipWidget()
{
    if (NULL != d_ptr) {
        delete d_ptr;
    }
}

void ViewPaperTipWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(340, 170, 120, 40, this);
}

void ViewPaperTipWidget::showEvent(QShowEvent *event)
{
    Q_D(ViewPaperTipWidget);
    if (NULL == d->m_VisibleTip) {
        d->m_VisibleTip = new QPixmap(QString(":/Images/hw_01_circle_tip_active.png"));
        d->m_Viewpaper0 = d->m_VisibleTip;
    }
    if (NULL == d->m_InvisibleTip) {
        d->m_InvisibleTip = new QPixmap(QString(":/Images/hw_01_circle_tip_inactive.png"));
        d->m_ViewPaper1 = d->m_InvisibleTip;
    }
}

void ViewPaperTipWidget::paintEvent(QPaintEvent *event)
{
    Q_D(ViewPaperTipWidget);
    QPainter painter(this);
        int x = 25;
        int y = 20;
        if (NULL != d->m_Viewpaper0) {
            painter.drawPixmap(x * g_Widget->widthScalabilityFactor(), y * g_Widget->heightScalabilityFactor(), *d->m_Viewpaper0);
        }
        if (NULL != d->m_ViewPaper1) {
            x += 50;
            painter.drawPixmap(x * g_Widget->widthScalabilityFactor(), y * g_Widget->heightScalabilityFactor(), *d->m_ViewPaper1);
        }
}

void ViewPaperTipWidget::onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status)
{
    Q_D(ViewPaperTipWidget);
    switch (destinationType) {
    case Widget::T_ViewPaper0: {
        if (WidgetStatus::Show == status) {
            d->m_Viewpaper0 = d->m_VisibleTip;
            d->m_ViewPaper1 = d->m_InvisibleTip;
            if (isVisible()) {
                update();
            }
        }
        break;
    }
    case Widget::T_ViewPaper1: {
        if (WidgetStatus::Show == status) {
            d->m_Viewpaper0 = d->m_InvisibleTip;
            d->m_ViewPaper1 = d->m_VisibleTip;
            if (isVisible()) {
                update();
            }
        }
        break;
    }
    default: {
        break;
    }
    }
}

ViewPaperTipWidgetPrivate::ViewPaperTipWidgetPrivate(ViewPaperTipWidget *parent)
    : q_ptr(parent)
{
    m_VisibleTip = NULL;
    m_InvisibleTip = NULL;
    m_Viewpaper0 = NULL;
    m_ViewPaper1 = NULL;
    initializeParent();
    initializeBasic();
}

ViewPaperTipWidgetPrivate::~ViewPaperTipWidgetPrivate()
{
    if (NULL != m_VisibleTip) {
        delete m_VisibleTip;
        m_VisibleTip = NULL;
    }
    if (NULL != m_InvisibleTip) {
        delete m_InvisibleTip;
        m_InvisibleTip = NULL;
    }
}

void ViewPaperTipWidgetPrivate::initializeParent()
{
    Q_Q(ViewPaperTipWidget);
    q->setVisible(true);
}

void ViewPaperTipWidgetPrivate::initializeBasic()
{
    Q_Q(ViewPaperTipWidget);
    connectSignalAndSlotByNamesake(g_Widget, q, ARKRECEIVER(onWidgetTypeChange(const Widget::Type, const Widget::Type, const QString &)));
}
