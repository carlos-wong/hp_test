#include "SettingWidget.h"
#include "AutoConnect.h"
#include "BmpWidget.h"
#include "BusinessLogic/Setting.h"
#include "SettingToolBarWidget/SettingToolBarWidget.h"
#include "SettingGeneralWidget/SettingGeneralWidget.h"
#include "SettingStyleWidget/SettingStyleWidget.h"
#include "SettingSoundWidget/SettingSoundWidget.h"
#include "SettingFactoryWidget/SettingFactoryWidget.h"

class SettingWidgetPrivate
{
public:
    explicit SettingWidgetPrivate(SettingWidget* parent);
    ~SettingWidgetPrivate();
    void initializeParent();
    void initializeBackgroundWidget();
    void initializeSettingToolBarWidget();
    void initializeSettingGeneralWidget();
    void initializeSettingStyleWidget();
    void initializeSettingSoundWidget();
    void initializeSettingFactory();
private:
    Q_DECLARE_PUBLIC(SettingWidget)
    SettingWidget* const q_ptr;
    Widget::Type m_Type;
    BmpWidget* m_Background;
    SettingToolBarWidget* m_SettingToolBarWidget;
    SettingGeneralWidget* m_SettingGeneralWidget;
    SettingStyleWidget* m_SettingStyleWidget;
    SettingSoundWidget* m_SettingSoundWidget;
    SettingFactoryWidget* m_SettingFactoryWidget;
};

SettingWidget::SettingWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new SettingWidgetPrivate(this))
{
}

SettingWidget::~SettingWidget()
{
    if (NULL != d_ptr) {
        delete d_ptr;
    }
}

void SettingWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(0, 0, 800, 480, this);
}

void SettingWidget::onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status)
{
    Q_D(SettingWidget);
    switch (destinationType) {
    case Widget::T_Home:
    case Widget::T_Back: {
        if (isVisible()) {
            if (WidgetStatus::RequestShow == status) {
                g_Widget->setWidgetType(Widget::T_ViewPaper1, Widget::T_Setting, WidgetStatus::RequestShow);
            }
        }
        break;
    }
    case Widget::T_Setting: {
        if (WidgetStatus::RequestShow == status) {
            g_Setting->requestMCUVersion();
            if (Widget::T_Undefine == d->m_Type) {
                d->initializeSettingGeneralWidget();
            } else {
                g_Widget->setWidgetType(d->m_Type, Widget::T_Setting, WidgetStatus::RequestShow);
            }
        }
        break;
    }
    case Widget::T_SettingGeneral: {
        if (WidgetStatus::Show == status) {
            d->initializeBackgroundWidget();
            d->initializeSettingToolBarWidget();
            if (!isVisible()) {
                setVisible(true);
            }
            d->m_Type = Widget::T_SettingGeneral;
        }
        break;
    }
    case Widget::T_SettingStyle: {
        if (WidgetStatus::RequestShow == status) {
            d->initializeSettingStyleWidget();
        } else if (WidgetStatus::Show == status) {
            d->m_Type = Widget::T_SettingStyle;
            if (!isVisible()) {
                setVisible(true);
            }
        }
        break;
    }
    case Widget::T_SettingSound: {
        if (WidgetStatus::RequestShow == status) {
            d->initializeSettingSoundWidget();
        } else if (WidgetStatus::Show == status) {
            d->m_Type = Widget::T_SettingSound;
            if (!isVisible()) {
                setVisible(true);
            }
        }
        break;
    }
    case Widget::T_SettingFactory: {
        if (WidgetStatus::RequestShow == status) {
            d->initializeSettingFactory();
        } else if (WidgetStatus::Show == status) {
            d->initializeBackgroundWidget();
            d->m_Background->setVisible(true);
            d->initializeSettingToolBarWidget();
            d->m_Type = destinationType;
            setVisible(true);
        }
        if (NULL != d->m_SettingToolBarWidget) {
            d->m_SettingToolBarWidget->raise();
        }
        break;
    }
    default: {
        if (WidgetStatus::Show == status) {
            if (isVisible()) {
                setVisible(false);
            }
        }
        break;
    }
    }
}

SettingWidgetPrivate::SettingWidgetPrivate(SettingWidget *parent)
    : q_ptr(parent)
{
    m_Type = Widget::T_Undefine;
    m_Background = NULL;
    m_SettingToolBarWidget = NULL;
    m_SettingGeneralWidget = NULL;
    m_SettingStyleWidget = NULL;
    m_SettingSoundWidget = NULL;
    m_SettingFactoryWidget = NULL;
    initializeParent();
}

SettingWidgetPrivate::~SettingWidgetPrivate()
{
}

void SettingWidgetPrivate::initializeParent()
{
    Q_Q(SettingWidget);
    q->setVisible(false);
    connectSignalAndSlotByNamesake(g_Widget, q, ARKRECEIVER(onWidgetTypeChange(const Widget::Type, const Widget::Type, const QString &)));
}

void SettingWidgetPrivate::initializeBackgroundWidget()
{
    if (NULL == m_Background) {
        Q_Q(SettingWidget);
        m_Background = new BmpWidget(q);
        m_Background->setBackgroundBmpPath(QString(":/Images/SettingWidgetBackground.png"));
        g_Widget->geometryFit(180, 52, 600, 334, m_Background);
        m_Background->lower();
        m_Background->setVisible(true);
    }
}

void SettingWidgetPrivate::initializeSettingGeneralWidget()
{
    if (NULL == m_SettingGeneralWidget) {
        Q_Q(SettingWidget);
        m_SettingGeneralWidget = new SettingGeneralWidget(q);
        g_Widget->setWidgetType(Widget::T_SettingGeneral, Widget::T_Setting, WidgetStatus::RequestShow);
    }
}

void SettingWidgetPrivate::initializeSettingStyleWidget()
{
    if (NULL == m_SettingStyleWidget) {
        Q_Q(SettingWidget);
        m_SettingStyleWidget = new SettingStyleWidget(q);
        g_Widget->setWidgetType(Widget::T_SettingStyle, Widget::T_Setting, WidgetStatus::RequestShow);
    }
}

void SettingWidgetPrivate::initializeSettingSoundWidget()
{
    if (NULL == m_SettingSoundWidget) {
        Q_Q(SettingWidget);
        m_SettingSoundWidget = new SettingSoundWidget(q);
        g_Widget->setWidgetType(Widget::T_SettingSound, Widget::T_Setting, WidgetStatus::RequestShow);
    }
}

void SettingWidgetPrivate::initializeSettingFactory()
{
    if (NULL == m_SettingFactoryWidget) {
        Q_Q(SettingWidget);
        m_SettingFactoryWidget = new SettingFactoryWidget(q);
        g_Widget->setWidgetType(Widget::T_SettingFactory, Widget::T_Setting, WidgetStatus::RequestShow);
    }
}

void SettingWidgetPrivate::initializeSettingToolBarWidget()
{
    if (NULL == m_SettingToolBarWidget) {
        Q_Q(SettingWidget);
        m_SettingToolBarWidget = new SettingToolBarWidget(q);
        m_SettingToolBarWidget->setVisible(true);
    }
}
