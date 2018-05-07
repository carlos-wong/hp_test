#include "MessageBoxWidget.h"
#include "BusinessLogic/Widget.h"
#include "UserInterfaceUtility.h"
#include "BusinessLogic/BusinessLogicUtility.h"
#include "UserInterfaceUtility.h"
#include <QEvent>
#include <QPainter>
#include <QVariant>

class MessageBoxWidgetPrivate
{
    Q_DISABLE_COPY(MessageBoxWidgetPrivate)
public:
    explicit MessageBoxWidgetPrivate(MessageBoxWidget *parent);
    ~MessageBoxWidgetPrivate();
    void initialize();
private:
    MessageBoxWidget* m_Parent = NULL;
};

MessageBoxWidget::MessageBoxWidget(QWidget *parent)
    : QWidget(parent)
    , m_Private(new MessageBoxWidgetPrivate(this))
{
}

MessageBoxWidget::~MessageBoxWidget()
{
}

void MessageBoxWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(0, 0, 800, 480, this);
}

MessageBoxWidgetPrivate::MessageBoxWidgetPrivate(MessageBoxWidget *parent)
    : m_Parent(parent)
{
    initialize();
}

MessageBoxWidgetPrivate::~MessageBoxWidgetPrivate()
{
}

void MessageBoxWidgetPrivate::initialize()
{
}
