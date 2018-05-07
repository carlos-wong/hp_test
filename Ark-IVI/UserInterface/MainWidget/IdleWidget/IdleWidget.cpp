#include "IdleWidget.h"
#include "BusinessLogic/Widget.h"
#include "BusinessLogic/BusinessLogicUtility.h"
#include "UserInterfaceUtility.h"
#include "BusinessLogic/Setting.h"
#include "AutoConnect.h"
#include <QPainter>
#include <QEvent>

class IdleWidgetPrivate
{
    Q_DISABLE_COPY(IdleWidgetPrivate)
public:
    explicit IdleWidgetPrivate(IdleWidget* parent);
    ~IdleWidgetPrivate();
    void initialize();
    void connectAllSlots();
private:
    IdleWidget* m_Parent = NULL;
};

IdleWidget::IdleWidget(QWidget *parent)
    : QWidget(parent)
    , m_Private(new IdleWidgetPrivate(this))
{
    setVisible(true);
}

IdleWidget::~IdleWidget()
{
}

void IdleWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
#ifdef gcc
    painter.fillRect(rect(), UserInterfaceUtility::customBlackColor());
#else
    painter.fillRect(rect(), Qt::transparent);
#endif
}

void IdleWidget::showEvent(QShowEvent *event)
{
    if (NULL != m_Private) {
        m_Private->connectAllSlots();
    }
}

void IdleWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(0, 0, 800, 480, this);
}

void IdleWidget::mouseReleaseEvent(QMouseEvent *event)
{
    g_Setting->setDisplayScreen(DST_On);
}

void IdleWidget::onDisplayScreenChange(const DisplayScreenType type)
{
    qDebug() << "IdleWidget::onDisplayScreenChange" << type;
    switch (type) {
    case DST_Off: {
        setVisible(true);
        break;
    }
    case DST_On: {
        setVisible(false);
        break;
    }
    default: {
        break;
    }
    }
}

IdleWidgetPrivate::IdleWidgetPrivate(IdleWidget *parent)
    : m_Parent(parent)
{
    initialize();
}

IdleWidgetPrivate::~IdleWidgetPrivate()
{
}

void IdleWidgetPrivate::initialize()
{
}

void IdleWidgetPrivate::connectAllSlots()
{
    connectSignalAndSlotByNamesake(g_Setting, m_Parent, SLOT(onDisplayScreenChange(const int)));
}
