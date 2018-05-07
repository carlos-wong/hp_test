#include "SettingLinkWidget.h"
#include "AutoConnect.h"
#include "BmpButton.h"

class SettingLinkWidgetPrivate
{
public:
    explicit SettingLinkWidgetPrivate(SettingLinkWidget* parent);
    ~SettingLinkWidgetPrivate();
    void initializeParent();
    void initializeBasicWidget();
private:
    Q_DECLARE_PUBLIC(SettingLinkWidget)
    SettingLinkWidget* const q_ptr;
    BmpButton* m_ModeOne = nullptr;
    BmpButton* m_ModeTwo = nullptr;
    BmpButton* m_ModeThree = nullptr;
};

SettingLinkWidget::SettingLinkWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new SettingLinkWidgetPrivate(this))
{
}

SettingLinkWidget::~SettingLinkWidget()
{
    if (nullptr != d_ptr) {
        delete d_ptr;
    }
}

void SettingLinkWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(0, 52, 800, 334, this);
}

void SettingLinkWidget::onWidgetTypeChange(const Widget::Type desitinationType, const Widget::Type requestType, const QString &status)
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
    case Widget::T_SettingStyle:
    case Widget::T_SettingSound:
    case Widget::T_SettingFactory: {
        if (WidgetStatus::Show == status) {
            if (isVisible()) {
                setVisible(false);
            }
        }
        break;
    }
    case Widget::T_SettingLink: {
        if (WidgetStatus::RequestShow == status) {
            g_Widget->setWidgetType(Widget::T_SettingLink, Widget::T_SettingLink, WidgetStatus::Show);
        } else if (WidgetStatus::Show == status) {
            Q_D(SettingLinkWidget);
            setVisible(true);
        }
        break;
    }
    default: {
        break;
    }
    }
}

SettingLinkWidgetPrivate::SettingLinkWidgetPrivate(SettingLinkWidget *parent)
    : q_ptr(parent)
{
    initializeParent();
    initializeBasicWidget();
}

SettingLinkWidgetPrivate::~SettingLinkWidgetPrivate()
{
}

void SettingLinkWidgetPrivate::initializeParent()
{
    Q_Q(SettingLinkWidget);
    q->setVisible(false);
    connectSignalAndSlotByNamesake(g_Widget, q, ARKRECEIVER(onWidgetTypeChange(const Widget::Type, const Widget::Type, const QString &)));
}

void SettingLinkWidgetPrivate::initializeBasicWidget()
{
    Q_Q(SettingLinkWidget);
    m_ModeOne = new BmpButton(q);
    m_ModeOne->setNormalBmpPath(QString(":/Images/SettingLinkWidgetModeOneNormal.png"));
    m_ModeOne->setPressBmpPath(QString(":/Images/SettingLinkWidgetModeOneCheck.png"));
    m_ModeOne->setCheckBmpPath(QString(":/Images/SettingLinkWidgetModeOneCheck.png"));
    g_Widget->geometryFit(68, 156, 176, 100, m_ModeOne);
    m_ModeOne->setVisible(true);

    m_ModeTwo = new BmpButton(q);
    m_ModeTwo->setNormalBmpPath(QString(":/Images/SettingLinkWidgetModeTwoNormal.png"));
    m_ModeTwo->setPressBmpPath(QString(":/Images/SettingLinkWidgetModeTwoCheck.png"));
    m_ModeTwo->setCheckBmpPath(QString(":/Images/SettingLinkWidgetModeTwoCheck.png"));
    g_Widget->geometryFit(312, 156, 176, 100, m_ModeTwo);
    m_ModeTwo->setVisible(true);

    m_ModeThree = new BmpButton(q);
    m_ModeThree->setNormalBmpPath(QString(":/Images/SettingLinkWidgetModeThreeNormal.png"));
    m_ModeThree->setPressBmpPath(QString(":/Images/SettingLinkWidgetModeThreeCheck.png"));
    m_ModeThree->setCheckBmpPath(QString(":/Images/SettingLinkWidgetModeThreeCheck.png"));
    g_Widget->geometryFit(556, 156, 176, 100, m_ModeThree);
    m_ModeThree->setVisible(true);
}


