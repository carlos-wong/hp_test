#include "SettingSoundToolWidget.h"
#include "AutoConnect.h"
#include "BusinessLogic/Widget.h"
#include "BmpButton.h"
#include <QPainter>

class SettingSoundToolWidgetPrivate
{
public:
    explicit SettingSoundToolWidgetPrivate(SettingSoundToolWidget* parent);
    ~SettingSoundToolWidgetPrivate();
    void initializeParent();
    void initializeBasicWidget();
private:
    Q_DECLARE_PUBLIC(SettingSoundToolWidget)
    SettingSoundToolWidget* const q_ptr;
    BmpButton* m_EqualizerBtn;
    BmpButton* m_SoundBtn;
};

SettingSoundToolWidget::SettingSoundToolWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new SettingSoundToolWidgetPrivate(this))
{
}

SettingSoundToolWidget::~SettingSoundToolWidget()
{
    if (NULL != d_ptr) {
        delete d_ptr;
    }
}

void SettingSoundToolWidget::setCheckBtn(const SettingSoundToolWidget::CheckBtn btn)
{
    Q_D(SettingSoundToolWidget);
    switch (btn) {
    case CB_Equalizer: {
        d->m_EqualizerBtn->setStatus(BmpButton::B_Check);
        d->m_SoundBtn->setStatus(BmpButton::B_Normal);
        break;
    }
    case CB_Sound: {
        d->m_EqualizerBtn->setStatus(BmpButton::B_Normal);
        d->m_SoundBtn->setStatus(BmpButton::B_Check);
        break;
    }
    default: {
        break;
    }
    }
}

void SettingSoundToolWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(0, 0, 180, 334, this);
}

void SettingSoundToolWidget::onToolButtonRelease()
{
    Q_D(SettingSoundToolWidget);
    BmpButton* ptr = static_cast<BmpButton*>(sender());
    if (ptr == d->m_EqualizerBtn) {
        toolButtonRelease(SettingSoundToolWidget::CB_Equalizer);
    } else if (ptr == d->m_SoundBtn) {
        toolButtonRelease(SettingSoundToolWidget::CB_Sound);
    }
}

SettingSoundToolWidgetPrivate::SettingSoundToolWidgetPrivate(SettingSoundToolWidget *parent)
    : q_ptr(parent)
{
    m_EqualizerBtn = NULL;
    m_SoundBtn = NULL;
    initializeParent();
    initializeBasicWidget();
}

SettingSoundToolWidgetPrivate::~SettingSoundToolWidgetPrivate()
{
}

void SettingSoundToolWidgetPrivate::initializeParent()
{
    Q_Q(SettingSoundToolWidget);
    q->setVisible(true);
}

void SettingSoundToolWidgetPrivate::initializeBasicWidget()
{
    Q_Q(SettingSoundToolWidget);
    Qt::ConnectionType type = static_cast<Qt::ConnectionType>(Qt::UniqueConnection | Qt::AutoConnection);
    m_EqualizerBtn = new BmpButton(q);
    m_EqualizerBtn->setNormalBmpPath(QString(":/Images/SettingSoundEqualizerNormal.png"));
    m_EqualizerBtn->setPressBmpPath(QString(":/Images/SettingSoundEqualizerPress.png"));
    m_EqualizerBtn->setCheckBmpPath(QString(":/Images/SettingSoundEqualizerPress.png"));
    g_Widget->geometryFit(24, 0, 138, 66, m_EqualizerBtn);
    m_EqualizerBtn->setVisible(true);
    QObject::connect(m_EqualizerBtn, ARKSENDER(released()),
                     q,              ARKRECEIVER(onToolButtonRelease()),
                     type);
    m_SoundBtn = new BmpButton(q);
    m_SoundBtn->setNormalBmpPath(QString(":/Images/SettingSoundSoundNormal.png"));
    m_SoundBtn->setPressBmpPath(QString(":/Images/SettingSoundSoundPress.png"));
    m_SoundBtn->setCheckBmpPath(QString(":/Images/SettingSoundSoundPress.png"));
    g_Widget->geometryFit(24, 67, 138, 66, m_SoundBtn);
    m_SoundBtn->setVisible(true);
    QObject::connect(m_SoundBtn, ARKSENDER(released()),
                     q,          ARKRECEIVER(onToolButtonRelease()),
                     type);
}

