#include "SettingGeneralWidget.h"
#include "AutoConnect.h"
#include "SettingGeneralDisplayWidget/SettingGeneralDisplayWidget.h"
#include "SettingGeneralVolumeWidget/SettingGeneralVolumeWidget.h"
#include "SettingGeneralLanguageWidget/SettingGeneralLanguageWidget.h"
#include "SettingGeneralTimeWidget/SettingGeneralTimeWidget.h"
#include "SettingGeneralVersionWidget/SettingGeneralVersionWidget.h"
#include <QPainter>

class SettingGeneralWidgetPrivate
{
public:
    explicit SettingGeneralWidgetPrivate(SettingGeneralWidget* parent);
    ~SettingGeneralWidgetPrivate();
    void initializeParent();
    void initializeToolWidget();
    void initializeSettingGeneralDisplayWidget();
    void initializeSettingGeneralVolumeWidget();
    void initializeSettingGeneralLanguageWidget();
    void initializeSettingGeneralTimeWidget();
    void initializeSettingGeneralVersionWidget();
private:
    Q_DECLARE_PUBLIC(SettingGeneralWidget)
    SettingGeneralWidget* const q_ptr;
    SettingGeneralToolWidget* m_SettingGeneralToolWidget;
    SettingGeneralDisplayWidget* m_SettingGeneralDisplayWidget;
    SettingGeneralVolumeWidget* m_SettingGeneralVolumeWidget;
    SettingGeneralLanguageWidget* m_SettingGeneralLanguageWidget;
    SettingGeneralTimeWidget* m_SettingGeneralTimeWidget;
    SettingGeneralVersionWidget* m_SettingGeneralVersionWidget;
};

SettingGeneralWidget::SettingGeneralWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new SettingGeneralWidgetPrivate(this))
{
}

SettingGeneralWidget::~SettingGeneralWidget()
{
    if (NULL != d_ptr) {
        delete d_ptr;
    }
}

void SettingGeneralWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(0, 52, 800, 428, this);
}

void SettingGeneralWidget::onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status)
{
    Q_D(SettingGeneralWidget);
    switch (destinationType) {
    case Widget::T_SettingGeneral: {
        if (WidgetStatus::RequestShow == status) {
            g_Widget->setWidgetType(Widget::T_SettingGeneral, Widget::T_SettingGeneral, WidgetStatus::Show);
        } else if (WidgetStatus::Show == status) {
            d->initializeToolWidget();
            d->initializeSettingGeneralDisplayWidget();
            setVisible(true);
        }
        break;
    }
    case Widget::T_SettingStyle:
    case Widget::T_SettingSound:
    case Widget::T_SettingFactory: {
        if (WidgetStatus::Show == status) {
            d->initializeSettingGeneralDisplayWidget();
            setVisible(false);
        }
        break;
    }
    default: {
        break;
    }
    }
}

void SettingGeneralWidget::onToolBtnRelease(const SettingGeneralToolWidget::CheckBtn btn)
{
    Q_D(SettingGeneralWidget);
    switch (btn) {
    case SettingGeneralToolWidget::CB_Dispaly: {
        d->m_SettingGeneralToolWidget->setCheckBtn(SettingGeneralToolWidget::CB_Dispaly);
        break;
    }
    case SettingGeneralToolWidget::CB_Volume: {
        d->initializeSettingGeneralVolumeWidget();
        d->m_SettingGeneralToolWidget->setCheckBtn(SettingGeneralToolWidget::CB_Volume);
        break;
    }
    case SettingGeneralToolWidget::CB_Language: {
        d->initializeSettingGeneralLanguageWidget();
        d->m_SettingGeneralToolWidget->setCheckBtn(SettingGeneralToolWidget::CB_Language);
        break;
    }
    case SettingGeneralToolWidget::CB_Time: {
        d->initializeSettingGeneralTimeWidget();
        d->m_SettingGeneralToolWidget->setCheckBtn(SettingGeneralToolWidget::CB_Time);
        break;
    }
    case SettingGeneralToolWidget::CB_Version: {
        d->initializeSettingGeneralVersionWidget();
        d->m_SettingGeneralToolWidget->setCheckBtn(SettingGeneralToolWidget::CB_Version);
        break;
    }
    default: {
        return ;
        break;
    }
    }
    if (NULL != d->m_SettingGeneralDisplayWidget) {
        d->m_SettingGeneralDisplayWidget->setVisible(SettingGeneralToolWidget::CB_Dispaly == btn);
    }
    if (NULL != d->m_SettingGeneralVolumeWidget) {
        d->m_SettingGeneralVolumeWidget->setVisible(SettingGeneralToolWidget::CB_Volume == btn);
    }
    if (NULL != d->m_SettingGeneralLanguageWidget) {
        d->m_SettingGeneralLanguageWidget->setVisible(SettingGeneralToolWidget::CB_Language == btn);
    }
    if (NULL != d->m_SettingGeneralTimeWidget) {
        d->m_SettingGeneralTimeWidget->setVisible(SettingGeneralToolWidget::CB_Time == btn);
    }
    if (NULL != d->m_SettingGeneralVersionWidget) {
        d->m_SettingGeneralVersionWidget->setVisible(SettingGeneralToolWidget::CB_Version == btn);
    }
}

SettingGeneralWidgetPrivate::SettingGeneralWidgetPrivate(SettingGeneralWidget *parent)
    : q_ptr(parent)
{
    m_SettingGeneralToolWidget = NULL;
    m_SettingGeneralDisplayWidget = NULL;
    m_SettingGeneralVolumeWidget = NULL;
    m_SettingGeneralLanguageWidget = NULL;
    m_SettingGeneralTimeWidget = NULL;
    m_SettingGeneralVersionWidget = NULL;
    initializeParent();
}

SettingGeneralWidgetPrivate::~SettingGeneralWidgetPrivate()
{
}

void SettingGeneralWidgetPrivate::initializeParent()
{
    Q_Q(SettingGeneralWidget);
    q->setVisible(false);
    connectSignalAndSlotByNamesake(g_Widget, q, ARKRECEIVER(onWidgetTypeChange(const Widget::Type, const Widget::Type, const QString &)));
}

void SettingGeneralWidgetPrivate::initializeToolWidget()
{
    Q_Q(SettingGeneralWidget);
    if (NULL == m_SettingGeneralToolWidget) {
        m_SettingGeneralToolWidget = new SettingGeneralToolWidget(q);
        m_SettingGeneralToolWidget->setCheckBtn(SettingGeneralToolWidget::CB_Dispaly);
        Qt::ConnectionType type = static_cast<Qt::ConnectionType>(Qt::UniqueConnection | Qt::AutoConnection);
        QObject::connect(m_SettingGeneralToolWidget, ARKSENDER(toolButtonRelease(const SettingGeneralToolWidget::CheckBtn)),
                         q,                          ARKRECEIVER(onToolBtnRelease(const SettingGeneralToolWidget::CheckBtn)),
                         type);
    }
}

void SettingGeneralWidgetPrivate::initializeSettingGeneralDisplayWidget()
{
    if (NULL == m_SettingGeneralDisplayWidget) {
        Q_Q(SettingGeneralWidget);
        m_SettingGeneralDisplayWidget = new SettingGeneralDisplayWidget(q);
        m_SettingGeneralDisplayWidget->setVisible(true);
    }
}

void SettingGeneralWidgetPrivate::initializeSettingGeneralVolumeWidget()
{
    if (NULL == m_SettingGeneralVolumeWidget) {
        Q_Q(SettingGeneralWidget);
        m_SettingGeneralVolumeWidget = new SettingGeneralVolumeWidget(q);
    }
}

void SettingGeneralWidgetPrivate::initializeSettingGeneralLanguageWidget()
{
    if (NULL == m_SettingGeneralLanguageWidget) {
        Q_Q(SettingGeneralWidget);
        m_SettingGeneralLanguageWidget = new SettingGeneralLanguageWidget(q);
    }
}

void SettingGeneralWidgetPrivate::initializeSettingGeneralTimeWidget()
{
    if (NULL == m_SettingGeneralTimeWidget) {
        Q_Q(SettingGeneralWidget);
        m_SettingGeneralTimeWidget = new SettingGeneralTimeWidget(q);
    }
}

void SettingGeneralWidgetPrivate::initializeSettingGeneralVersionWidget()
{
    if (NULL == m_SettingGeneralVersionWidget) {
        Q_Q(SettingGeneralWidget);
        m_SettingGeneralVersionWidget = new SettingGeneralVersionWidget(q);
    }
}
