#include "SettingFactoryToolWidget.h"
#include "BusinessLogic/Widget.h"
#include "BmpButton.h"
#include "AutoConnect.h"

class SettingFactoryToolWidgetPrivate
{
    Q_DISABLE_COPY(SettingFactoryToolWidgetPrivate)
public:
    explicit SettingFactoryToolWidgetPrivate(SettingFactoryToolWidget* parent);
    ~SettingFactoryToolWidgetPrivate();
    void initializeParent();
    void initializeBasicWidget();
private:
    Q_DECLARE_PUBLIC(SettingFactoryToolWidget)
    SettingFactoryToolWidget* q_ptr;
    BmpButton* m_UpdateLinkBtn;
    BmpButton* m_UpdateLogoBtn;
};

SettingFactoryToolWidget::SettingFactoryToolWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new SettingFactoryToolWidgetPrivate(this))
{
}

SettingFactoryToolWidget::~SettingFactoryToolWidget()
{
    if (NULL != d_ptr) {
        delete d_ptr;
    }
}

void SettingFactoryToolWidget::setCheckBtn(const SettingFactoryToolWidget::CheckBtn btn)
{
    Q_D(SettingFactoryToolWidget);
    switch (btn) {
    case CB_UpdateLink: {
        d->m_UpdateLinkBtn->setStatus(BmpButton::B_Check);
        d->m_UpdateLogoBtn->setStatus(BmpButton::B_Normal);
        break;
    }
    case CB_UpdateLogo: {
        d->m_UpdateLinkBtn->setStatus(BmpButton::B_Normal);
        d->m_UpdateLogoBtn->setStatus(BmpButton::B_Check);
        break;
    }
    default: {
        break;
    }
    }
}

void SettingFactoryToolWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(0, 52, 180, 334, this);
}

void SettingFactoryToolWidget::onToolButtonRelease()
{
    Q_D(SettingFactoryToolWidget);
    BmpButton* ptr = static_cast<BmpButton*>(sender());
    if (ptr == d->m_UpdateLinkBtn) {
        toolButtonRelease(SettingFactoryToolWidget::CB_UpdateLink);
    } else if (ptr == d->m_UpdateLogoBtn) {
        toolButtonRelease(SettingFactoryToolWidget::CB_UpdateLogo);
    }
}

SettingFactoryToolWidgetPrivate::SettingFactoryToolWidgetPrivate(SettingFactoryToolWidget *parent)
    : q_ptr(parent)
{
    m_UpdateLinkBtn = NULL;
    m_UpdateLogoBtn = NULL;
    initializeParent();
    initializeBasicWidget();
}

SettingFactoryToolWidgetPrivate::~SettingFactoryToolWidgetPrivate()
{
}

void SettingFactoryToolWidgetPrivate::initializeParent()
{
    Q_Q(SettingFactoryToolWidget);
    q->setVisible(true);
}

void SettingFactoryToolWidgetPrivate::initializeBasicWidget()
{
    Q_Q(SettingFactoryToolWidget);
    Qt::ConnectionType type = static_cast<Qt::ConnectionType>(Qt::UniqueConnection | Qt::AutoConnection);
    m_UpdateLinkBtn = new BmpButton(q);
    m_UpdateLinkBtn->setNormalBmpPath(QString(":/Images/SettingFactoryLinkNormal.png"));
    m_UpdateLinkBtn->setPressBmpPath(QString(":/Images/SettingFactoryLinkPress.png"));
    m_UpdateLinkBtn->setCheckBmpPath(QString(":/Images/SettingFactoryLinkPress.png"));
    g_Widget->geometryFit(24, 0, 138, 66, m_UpdateLinkBtn);
    m_UpdateLinkBtn->setVisible(true);
    QObject::connect(m_UpdateLinkBtn, ARKSENDER(released()),
                     q,               ARKRECEIVER(onToolButtonRelease()),
                     type);

    m_UpdateLogoBtn = new BmpButton(q);
    m_UpdateLogoBtn->setNormalBmpPath(QString(":/Images/SettingFactoryLogoNormal.png"));
    m_UpdateLogoBtn->setPressBmpPath(QString(":/Images/SettingFactoryLogoPress.png"));
    m_UpdateLogoBtn->setCheckBmpPath(QString(":/Images/SettingFactoryLogoPress.png"));
    g_Widget->geometryFit(24, 67, 138, 66, m_UpdateLogoBtn);
    m_UpdateLogoBtn->setVisible(true);
    QObject::connect(m_UpdateLogoBtn, ARKSENDER(released()),
                     q,               ARKRECEIVER(onToolButtonRelease()),
                     type);
}
