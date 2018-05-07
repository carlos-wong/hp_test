#include "SettingStyleWidget.h"
#include "AutoConnect.h"
#include "SettingStyleWallpaperWidget/SettingStyleWallpaperWidget.h"

class SettingStyleWidgetPrivate
{
public:
    explicit SettingStyleWidgetPrivate(SettingStyleWidget* parent);
    ~SettingStyleWidgetPrivate();
    void initializeParent();
    void initializeToolWidget();
    void initializeStyleWallpaperWidget();
private:
    Q_DECLARE_PUBLIC(SettingStyleWidget)
    SettingStyleWidget* const q_ptr;
    SettingStyleToolWidget* m_SettingStyleToolWidget;
    SettingStyleWallpaperWidget* m_SettingStyleWallpaperWidget;
};

SettingStyleWidget::SettingStyleWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new SettingStyleWidgetPrivate(this))
{
}

SettingStyleWidget::~SettingStyleWidget()
{
    if (NULL != d_ptr) {
        delete d_ptr;
    }
}

void SettingStyleWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(0, 52, 800, 334, this);
}

void SettingStyleWidget::onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status)
{
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
    case Widget::T_ViewPaper0:
    case Widget::T_ViewPaper1:
    case Widget::T_SettingGeneral:
    case Widget::T_SettingSound:
    case Widget::T_SettingFactory: {
        if (WidgetStatus::Show == status) {
            if (isVisible()) {
                setVisible(false);
            }
        }
        break;
    }
    case Widget::T_SettingStyle: {
        if (WidgetStatus::RequestShow == status) {
            g_Widget->setWidgetType(Widget::T_SettingStyle, Widget::T_SettingStyle, WidgetStatus::Show);
        } else if (WidgetStatus::Show == status) {
            Q_D(SettingStyleWidget);
            d->initializeToolWidget();
            d->initializeStyleWallpaperWidget();
            setVisible(true);
        }
        break;
    }
    default: {
        break;
    }
    }
}

void SettingStyleWidget::onToolBtnRelease(const SettingStyleToolWidget::CheckBtn btn)
{
    Q_D(SettingStyleWidget);
    switch (btn) {
    case SettingStyleToolWidget::CB_Wallpaper: {
        d->m_SettingStyleToolWidget->setCheckBtn(SettingStyleToolWidget::CB_Wallpaper);
        break;
    }
    default: {
        break;
    }
    }
    if (NULL != d->m_SettingStyleWallpaperWidget) {
        d->m_SettingStyleWallpaperWidget->setVisible(SettingStyleToolWidget::CB_Wallpaper == btn);
    }
}

SettingStyleWidgetPrivate::SettingStyleWidgetPrivate(SettingStyleWidget *parent)
    : q_ptr(parent)
{
    m_SettingStyleToolWidget = NULL;
    m_SettingStyleWallpaperWidget = NULL;
    initializeParent();
}

SettingStyleWidgetPrivate::~SettingStyleWidgetPrivate()
{
}

void SettingStyleWidgetPrivate::initializeParent()
{
    Q_Q(SettingStyleWidget);
    q->setVisible(false);
    connectSignalAndSlotByNamesake(g_Widget, q, ARKRECEIVER(onWidgetTypeChange(const Widget::Type, const Widget::Type, const QString &)));
}

void SettingStyleWidgetPrivate::initializeToolWidget()
{
    if (NULL == m_SettingStyleToolWidget) {
        Q_Q(SettingStyleWidget);
        m_SettingStyleToolWidget = new SettingStyleToolWidget(q);
        m_SettingStyleToolWidget->setCheckBtn(SettingStyleToolWidget::CB_Wallpaper);
        m_SettingStyleToolWidget->setVisible(true);
        QObject::connect(m_SettingStyleToolWidget, ARKSENDER(toolButtonRelease(const SettingStyleToolWidget::CheckBtn)),
                         q,                        ARKRECEIVER(onToolBtnRelease(const SettingStyleToolWidget::CheckBtn)));
    }
}

void SettingStyleWidgetPrivate::initializeStyleWallpaperWidget()
{
    if (NULL == m_SettingStyleWallpaperWidget) {
        Q_Q(SettingStyleWidget);
        m_SettingStyleWallpaperWidget = new SettingStyleWallpaperWidget(q);
        m_SettingStyleWallpaperWidget->setVisible(true);
    }
}

