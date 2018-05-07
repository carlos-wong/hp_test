#include "SettingEqualizerWidget.h"
#include "AutoConnect.h"

class SettingEqualizerWidgetPrivate
{
public:
    explicit SettingEqualizerWidgetPrivate(SettingEqualizerWidget* parent);
    ~SettingEqualizerWidgetPrivate();
    void initializeParent();
private:
    Q_DECLARE_PUBLIC(SettingEqualizerWidget)
    SettingEqualizerWidget* const q_ptr;
};

SettingEqualizerWidget::SettingEqualizerWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new SettingEqualizerWidgetPrivate(this))
{
}

SettingEqualizerWidget::~SettingEqualizerWidget()
{
    if (NULL != d_ptr) {
        delete d_ptr;
    }
}

void SettingEqualizerWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(0, 52, 800, 334, this);
}

void SettingEqualizerWidget::onWidgetTypeChange(const Widget::Type desitinationType, const Widget::Type requestType, const QString &status)
{
    switch (desitinationType) {
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
    case Widget::T_SettingLink:
    case Widget::T_SettingStyle:
    case Widget::T_SettingFactory: {
        if (WidgetStatus::Show == status) {
            if (isVisible()) {
                setVisible(false);
            }
        }
        break;
    }
    case Widget::T_SettingEqualizer: {
        if (WidgetStatus::RequestShow == status) {
            g_Widget->setWidgetType(Widget::T_SettingEqualizer, Widget::T_SettingEqualizer, WidgetStatus::Show);
        } else if (WidgetStatus::Show == status) {
            Q_D(SettingEqualizerWidget);
            setVisible(true);
        }
        break;
    }
    default: {
        break;
    }
    }
}

SettingEqualizerWidgetPrivate::SettingEqualizerWidgetPrivate(SettingEqualizerWidget *parent)
    : q_ptr(parent)
{
    initializeParent();
}

SettingEqualizerWidgetPrivate::~SettingEqualizerWidgetPrivate()
{
}

void SettingEqualizerWidgetPrivate::initializeParent()
{
    Q_Q(SettingEqualizerWidget);
    q->setVisible(false);
    connectSignalAndSlotByNamesake(g_Widget, q, SLOT(onWidgetTypeChange(const Widget::Type, const Widget::Type, const QString &)));
}
