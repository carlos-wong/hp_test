#include "BrandMiniWidget.h"
#include "BusinessLogic/Widget.h"
#include "BmpWidget.h"
#include "TextWidget.h"
#include "BmpButton.h"
#include "AutoConnect.h"
#include "BusinessLogic/Audio.h"
#include <QPainter>
#include <QDebug>

class BrandMiniWidgetPrivate
{
public:
    explicit BrandMiniWidgetPrivate(BrandMiniWidget* parent);
    ~BrandMiniWidgetPrivate();
    void initializeParent();
    void initializeBasic();
    void connectAllSlots();
private:
    Q_DECLARE_PUBLIC(BrandMiniWidget)
    BrandMiniWidget* const q_ptr;
    BmpWidget* m_Background;
};

BrandMiniWidget::BrandMiniWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new BrandMiniWidgetPrivate(this))
{
}

BrandMiniWidget::~BrandMiniWidget()
{
    if (NULL != d_ptr) {
        delete d_ptr;
    }
}

void BrandMiniWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(468, 77, 235, 200, this);
}

BrandMiniWidgetPrivate::BrandMiniWidgetPrivate(BrandMiniWidget *parent)
    : q_ptr(parent)
{
    m_Background = NULL;
    initializeBasic();
    initializeParent();
    connectAllSlots();
}

BrandMiniWidgetPrivate::~BrandMiniWidgetPrivate()
{
}

void BrandMiniWidgetPrivate::initializeParent()
{
    Q_Q(BrandMiniWidget);
    q->setVisible(false);
}

void BrandMiniWidgetPrivate::initializeBasic()
{
    Q_Q(BrandMiniWidget);
    m_Background = new BmpWidget(q);
    m_Background->setBackgroundBmpPath(QString(":/Images/BrandMiniWidgetBackground.png"));
    g_Widget->geometryFit(0, 0, 235, 194, m_Background);
    m_Background->setVisible(true);
}

void BrandMiniWidgetPrivate::connectAllSlots()
{
}
