#include "SettingSoundWidget.h"
#include "SettingEqualizerWidget/SettingEqualizerWidget.h"
#include "SettingSoundSoundWidget/SettingSoundSoundWidget.h"
#include "AutoConnect.h"

class SettingSoundWidgetPrivate
{
public:
    explicit SettingSoundWidgetPrivate(SettingSoundWidget* parent);
    ~SettingSoundWidgetPrivate();
    void initializeParent();
    void initializeEqualizerWidget();
    void initializeSoundSoundWidget();
    void initializeToolBar();
private:
    Q_DECLARE_PUBLIC(SettingSoundWidget)
    SettingSoundWidget* const q_ptr;
    SettingEqualizerWidget* m_SettingEqualizerWidget;
    SettingSoundSoundWidget* m_SettingSoundSoundWidget;
    SettingSoundToolWidget* m_SettingSoundToolWidget;
};

SettingSoundWidget::SettingSoundWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new SettingSoundWidgetPrivate(this))
{
}

SettingSoundWidget::~SettingSoundWidget()
{
    if (NULL != d_ptr) {
        delete d_ptr;
    }
}

void SettingSoundWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(0, 52, 800, 334, this);
}

void SettingSoundWidget::onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status)
{
    switch (destinationType) {
    case Widget::T_ViewPaper0:
    case Widget::T_ViewPaper1:
    case Widget::T_SettingGeneral:
    case Widget::T_SettingStyle:
    case Widget::T_SettingFactory: {
        if (WidgetStatus::Show == status) {
            if (isVisible()) {
                setVisible(false);
            }
        }
        break;
    }
    case Widget::T_SettingSound: {
        if (WidgetStatus::RequestShow == status) {
            g_Widget->setWidgetType(Widget::T_SettingSound, Widget::T_SettingSound, WidgetStatus::Show);
        } else if (WidgetStatus::Show == status) {
            Q_D(SettingSoundWidget);
            d->initializeToolBar();
            d->initializeEqualizerWidget();
            setVisible(true);
        }
        break;
    }
    default: {
        break;
    }
    }
}

void SettingSoundWidget::onToolBtnRelease(const SettingSoundToolWidget::CheckBtn btn)
{
    Q_D(SettingSoundWidget);
    switch (btn) {
    case SettingSoundToolWidget::CB_Equalizer: {
        d->m_SettingEqualizerWidget->setVisible(true);
        if (NULL != d->m_SettingSoundSoundWidget) {
            d->m_SettingSoundSoundWidget->setVisible(false);
        }
        d->m_SettingSoundToolWidget->setCheckBtn(SettingSoundToolWidget::CB_Equalizer);
        break;
    }
    case SettingSoundToolWidget::CB_Sound: {
        d->initializeSoundSoundWidget();
        d->m_SettingSoundSoundWidget->setVisible(true);
        d->m_SettingEqualizerWidget->setVisible(false);
        d->m_SettingSoundToolWidget->setCheckBtn(SettingSoundToolWidget::CB_Sound);
        break;
    }
    default: {
        break;
    }
    }
}

SettingSoundWidgetPrivate::SettingSoundWidgetPrivate(SettingSoundWidget *parent)
    : q_ptr(parent)
{
    m_SettingEqualizerWidget = NULL;
    m_SettingSoundSoundWidget = NULL;
    m_SettingSoundToolWidget = NULL;
    initializeParent();
}

SettingSoundWidgetPrivate::~SettingSoundWidgetPrivate()
{
}

void SettingSoundWidgetPrivate::initializeParent()
{
    Q_Q(SettingSoundWidget);
    q->setVisible(false);
    connectSignalAndSlotByNamesake(g_Widget, q, ARKRECEIVER(onWidgetTypeChange(const Widget::Type, const Widget::Type, const QString &)));
}

void SettingSoundWidgetPrivate::initializeEqualizerWidget()
{
    if (NULL == m_SettingEqualizerWidget) {
        Q_Q(SettingSoundWidget);
        m_SettingEqualizerWidget = new SettingEqualizerWidget(q);
        m_SettingEqualizerWidget->setVisible(true);
    }
}

void SettingSoundWidgetPrivate::initializeSoundSoundWidget()
{
    if (NULL == m_SettingSoundSoundWidget) {
        Q_Q(SettingSoundWidget);
        m_SettingSoundSoundWidget = new SettingSoundSoundWidget(q);
        m_SettingSoundSoundWidget->setVisible(true);
    }
}

void SettingSoundWidgetPrivate::initializeToolBar()
{
    if (NULL == m_SettingSoundToolWidget) {
        Q_Q(SettingSoundWidget);
        m_SettingSoundToolWidget = new SettingSoundToolWidget(q);
        m_SettingSoundToolWidget->setCheckBtn(SettingSoundToolWidget::CB_Equalizer);
        QObject::connect(m_SettingSoundToolWidget, ARKSENDER(toolButtonRelease(const SettingSoundToolWidget::CheckBtn)),
                         q,                        ARKRECEIVER(onToolBtnRelease(const SettingSoundToolWidget::CheckBtn)));
    }
}
