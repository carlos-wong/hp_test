#include "LinkMessageTipWidget.h"
#include "AutoConnect.h"
#include "Widget.h"
#include "MessageBox.h"
#include <QDebug>

class LinkMessageTipWidgetPrivate
{
    Q_DISABLE_COPY(LinkMessageTipWidgetPrivate)
public:
    LinkMessageTipWidgetPrivate(LinkMessageTipWidget* parent);
    ~LinkMessageTipWidgetPrivate();
    void initializeParent();
    void initializeBasicWidget();
    void connectAllSlots();
private:
    Q_DECLARE_PUBLIC(LinkMessageTipWidget)
    LinkMessageTipWidget* const q_ptr = NULL;
    MessageBox* m_MessageBox = NULL;
};

LinkMessageTipWidget::LinkMessageTipWidget(QWidget *parent)
    : QWidget(parent)
    , Link::Interface()
    , d_ptr(new LinkMessageTipWidgetPrivate(this))
{
    setStyleSheet("background-color: black;");
    Q_D(LinkMessageTipWidget);
    d->initializeParent();
}

LinkMessageTipWidget::~LinkMessageTipWidget()
{
    Q_D(LinkMessageTipWidget);
    delete d;
}

void LinkMessageTipWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(0, 0, 800, 480, this);
}

void LinkMessageTipWidget::onLinkStatusChange(const Link_Type type, const Link_STATUS status)
{
    qWarning() << __PRETTY_FUNCTION__ << type << status;
    Q_D(LinkMessageTipWidget);
    d->initializeBasicWidget();
    d->m_MessageBox->setText(SourceString::No_Mirror_Device);
    setVisible(true);
}

LinkMessageTipWidgetPrivate::LinkMessageTipWidgetPrivate(LinkMessageTipWidget *parent)
    : q_ptr(parent)
{
    connectAllSlots();
}

LinkMessageTipWidgetPrivate::~LinkMessageTipWidgetPrivate()
{
}

void LinkMessageTipWidgetPrivate::initializeParent()
{
    Q_Q(LinkMessageTipWidget);
    q->setVisible(false);
}

void LinkMessageTipWidgetPrivate::initializeBasicWidget()
{
    if (NULL == m_MessageBox) {
        Q_Q(LinkMessageTipWidget);
        m_MessageBox = new MessageBox(q);
        g_Widget->geometryFit(0, 0, 800, 480, m_MessageBox);
    }
}

void LinkMessageTipWidgetPrivate::connectAllSlots()
{
    Q_Q(LinkMessageTipWidget);
    connectSignalAndSlotByNamesake(g_Link, q);
}
