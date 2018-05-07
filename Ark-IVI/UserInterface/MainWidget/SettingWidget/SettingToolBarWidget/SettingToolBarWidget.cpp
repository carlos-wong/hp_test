#include "SettingToolBarWidget.h"
#include "AutoConnect.h"
#include "BmpWidget.h"
#include "BmpButton.h"

class SettingToolBarWidgetPrivate
{
public:
    explicit SettingToolBarWidgetPrivate(SettingToolBarWidget* parent);
    ~SettingToolBarWidgetPrivate();
    void initializeParent();
    void initializeBasicWidget();
    void connectAllSlots();
private:
    Q_DECLARE_PUBLIC(SettingToolBarWidget)
    SettingToolBarWidget* const q_ptr;
    BmpWidget* m_Background;
    BmpButton* m_General;
    BmpButton* m_Style;
    BmpButton* m_Equalizer;
    BmpButton* m_Factory;
};

SettingToolBarWidget::SettingToolBarWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new SettingToolBarWidgetPrivate(this))
{
}

SettingToolBarWidget::~SettingToolBarWidget()
{
    if (NULL != d_ptr) {
        delete d_ptr;
    }
}

void SettingToolBarWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(0, 387, 800, 94, this);
}

void SettingToolBarWidget::onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status)
{
    if (WidgetStatus::Show == status) {
        Q_D(SettingToolBarWidget);
        switch (destinationType) {
        case Widget::T_SettingGeneral: {
            d->m_General->setStatus(BmpButton::B_Check);
            d->m_Style->setStatus(BmpButton::B_Normal);
            d->m_Equalizer->setStatus(BmpButton::B_Normal);
            d->m_Factory->setStatus(BmpButton::B_Normal);
            break;
        }
        case Widget::T_SettingStyle: {
            d->m_General->setStatus(BmpButton::B_Normal);
            d->m_Style->setStatus(BmpButton::B_Check);
            d->m_Equalizer->setStatus(BmpButton::B_Normal);
            d->m_Factory->setStatus(BmpButton::B_Normal);
            break;
        }
        case Widget::T_SettingSound: {
            d->m_General->setStatus(BmpButton::B_Normal);
            d->m_Style->setStatus(BmpButton::B_Normal);
            d->m_Equalizer->setStatus(BmpButton::B_Check);
            d->m_Factory->setStatus(BmpButton::B_Normal);
            break;
        }
        case Widget::T_SettingFactory: {
            d->m_General->setStatus(BmpButton::B_Normal);
            d->m_Style->setStatus(BmpButton::B_Normal);
            d->m_Equalizer->setStatus(BmpButton::B_Normal);
            d->m_Factory->setStatus(BmpButton::B_Check);
            break;
        }
        default: {
            break;
        }
        }
    }
}

void SettingToolBarWidget::onToolBarBtnRelease()
{
    BmpButton* ptr = static_cast<BmpButton*>(sender());
    Q_D(SettingToolBarWidget);
    if (ptr == d->m_General) {
        g_Widget->setWidgetType(Widget::T_SettingGeneral, Widget::T_Setting, WidgetStatus::RequestShow);
    } else if (ptr == d->m_Style) {
        g_Widget->setWidgetType(Widget::T_SettingStyle, Widget::T_Setting, WidgetStatus::RequestShow);
    } else if (ptr == d->m_Equalizer) {
        g_Widget->setWidgetType(Widget::T_SettingSound, Widget::T_Setting, WidgetStatus::RequestShow);
    } else if (ptr == d->m_Factory) {
        g_Widget->setWidgetType(Widget::T_SettingFactory, Widget::T_Setting, WidgetStatus::RequestShow);
    }
}

SettingToolBarWidgetPrivate::SettingToolBarWidgetPrivate(SettingToolBarWidget *parent)
    : q_ptr(parent)
{
    m_Background = NULL;
    m_General = NULL;
    m_Style = NULL;
    m_Equalizer = NULL;
    m_Factory = NULL;
    initializeBasicWidget();
    initializeParent();
}

SettingToolBarWidgetPrivate::~SettingToolBarWidgetPrivate()
{
}

void SettingToolBarWidgetPrivate::initializeParent()
{
    Q_Q(SettingToolBarWidget);
    q->setVisible(false);
    connectSignalAndSlotByNamesake(g_Widget, q, ARKRECEIVER(onWidgetTypeChange(const Widget::Type, const Widget::Type, const QString &)));
}

void SettingToolBarWidgetPrivate::initializeBasicWidget()
{
    Q_Q(SettingToolBarWidget);
    m_Background = new BmpWidget(q);
    m_Background->setBackgroundBmpPath(QString(":/Images/SettingToolBarWidgetBackground.png"));
    g_Widget->geometryFit(0, 0, 800, 94, m_Background);
    m_Background->setVisible(true);

    m_General = new BmpButton(q);
    m_General->setNormalBmpPath(QString(":/Images/SettingToolBarWidgetGeneralNormal.png"));
    m_General->setPressBmpPath(QString(":/Images/SettingToolBarWidgetGeneralPress.png"));
    m_General->setCheckBmpPath(QString(":/Images/SettingToolBarWidgetGeneralPress.png"));
    g_Widget->geometryFit(0, 14, 200, 80, m_General);
    m_General->setStatus(BmpButton::B_Check);
    m_General->setVisible(true);
    m_Style = new BmpButton(q);
    m_Style->setNormalBmpPath(QString(":/Images/SettingToolBarWidgetStyleNormal.png"));
    m_Style->setPressBmpPath(QString(":/Images/SettingToolBarWidgetStylePress.png"));
    m_Style->setCheckBmpPath(QString(":/Images/SettingToolBarWidgetStylePress.png"));
    g_Widget->geometryFit(200, 14, 200, 80, m_Style);
    m_Style->setVisible(true);
    m_Equalizer = new BmpButton(q);
    m_Equalizer->setNormalBmpPath(QString(":/Images/SettingToolBarWidgetEqualizerNormal.png"));
    m_Equalizer->setPressBmpPath(QString(":/Images/SettingToolBarWidgetEqualizerPress.png"));
    m_Equalizer->setCheckBmpPath(QString(":/Images/SettingToolBarWidgetEqualizerPress.png"));
    g_Widget->geometryFit(400, 14, 200, 80, m_Equalizer);
    m_Equalizer->setVisible(true);
    m_Factory = new BmpButton(q);
    m_Factory->setNormalBmpPath(QString(":/Images/SettingToolBarWidgetFactoryNormal.png"));
    m_Factory->setPressBmpPath(QString(":/Images/SettingToolBarWidgetFactoryPress.png"));
    m_Factory->setCheckBmpPath(QString(":/Images/SettingToolBarWidgetFactoryPress.png"));
    g_Widget->geometryFit(600, 14, 200, 80, m_Factory);
    m_Factory->setVisible(true);

    connectAllSlots();
}

void SettingToolBarWidgetPrivate::connectAllSlots()
{
    Qt::ConnectionType type = static_cast<Qt::ConnectionType>(Qt::AutoConnection | Qt::UniqueConnection);
    Q_Q(SettingToolBarWidget);
    QObject::connect(m_General, ARKSENDER(released()),
                     q,         ARKRECEIVER(onToolBarBtnRelease()),
                     type);
    QObject::connect(m_Style, ARKSENDER(released()),
                     q,       ARKRECEIVER(onToolBarBtnRelease()),
                     type);
    QObject::connect(m_Equalizer, ARKSENDER(released()),
                     q,           ARKRECEIVER(onToolBarBtnRelease()),
                     type);
    QObject::connect(m_Factory, ARKSENDER(released()),
                     q,         ARKRECEIVER(onToolBarBtnRelease()),
                     type);
}
