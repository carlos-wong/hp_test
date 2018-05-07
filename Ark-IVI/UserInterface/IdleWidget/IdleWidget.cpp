#include "IdleWidget.h"
#include "BusinessLogic/Widget.h"
#include "BusinessLogic/BusinessLogicUtility.h"
#include "UserInterfaceUtility.h"
#include "BusinessLogic/Setting.h"
#include "AutoConnect.h"
#include <QPainter>
#include <QMouseEvent>
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
    IdleWidget* m_Parent;
};

IdleWidget::IdleWidget(QWidget *parent)
    : QWidget(parent)
    , m_Private(new IdleWidgetPrivate(this))
{
}

IdleWidget::~IdleWidget()
{
}

void IdleWidget::onArkKeyHandler(const ArkKey *key)
{
    if ((AKS_Release == key->status)
            || (AKS_Active == key->status)) {
        switch (key->type) {
        case AKT_CommonPower: {
            if (!isVisible()) {
                g_Setting->setDisplayScreen(DST_Off);
            } else {
                g_Setting->setDisplayScreen(DST_On);
            }
            break;
        }
        default: {
            break;
        }
        }
    }
}

void IdleWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(0, 0, 800, 480, this);
}

void IdleWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (rect().contains(event->pos())) {
        g_Setting->setDisplayScreen(DST_On);
    }
}

void IdleWidget::onDisplayScreenChange(const DisplayScreenType type)
{
    switch (type) {
    case DST_Off: {
        raise();
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
    connectAllSlots();
}

IdleWidgetPrivate::~IdleWidgetPrivate()
{
}

void IdleWidgetPrivate::initialize()
{
    m_Parent->setVisible(false);
}

void IdleWidgetPrivate::connectAllSlots()
{
    connectSignalAndSlotByNamesake(g_Setting, m_Parent, ARKRECEIVER(onDisplayScreenChange(const int)));
}
