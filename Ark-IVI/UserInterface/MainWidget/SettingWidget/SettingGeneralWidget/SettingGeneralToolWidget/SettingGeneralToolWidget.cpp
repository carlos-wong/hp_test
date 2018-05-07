#include "SettingGeneralToolWidget.h"
#include "AutoConnect.h"
#include "BmpButton.h"
#include "BusinessLogic/Widget.h"
#include <QPainter>

class SettingGeneralToolWidgetPrivate
{
public:
    explicit SettingGeneralToolWidgetPrivate(SettingGeneralToolWidget* parent);
    ~SettingGeneralToolWidgetPrivate();
    void initializeParent();
    void initializeBasicWidget();
private:
    Q_DECLARE_PUBLIC(SettingGeneralToolWidget)
    SettingGeneralToolWidget* const q_ptr;
    BmpButton* m_Display;
    BmpButton* m_Volume;
    BmpButton* m_Language;
    BmpButton* m_Time;
    BmpButton* m_Version;
};

SettingGeneralToolWidget::SettingGeneralToolWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new SettingGeneralToolWidgetPrivate(this))
{
}

SettingGeneralToolWidget::~SettingGeneralToolWidget()
{
    if (NULL != d_ptr) {
        delete d_ptr;
    }
}

void SettingGeneralToolWidget::setCheckBtn(const SettingGeneralToolWidget::CheckBtn btn)
{
    Q_D(SettingGeneralToolWidget);
    switch (btn) {
    case CB_Dispaly: {
        d->m_Display->setStatus(BmpButton::B_Check);
        d->m_Volume->setStatus(BmpButton::B_Normal);
        d->m_Language->setStatus(BmpButton::B_Normal);
        d->m_Time->setStatus(BmpButton::B_Normal);
        d->m_Version->setStatus(BmpButton::B_Normal);
        break;
    }
    case CB_Volume: {
        d->m_Display->setStatus(BmpButton::B_Normal);
        d->m_Volume->setStatus(BmpButton::B_Check);
        d->m_Language->setStatus(BmpButton::B_Normal);
        d->m_Time->setStatus(BmpButton::B_Normal);
        d->m_Version->setStatus(BmpButton::B_Normal);
        break;
    }
    case CB_Language: {
        d->m_Display->setStatus(BmpButton::B_Normal);
        d->m_Volume->setStatus(BmpButton::B_Normal);
        d->m_Language->setStatus(BmpButton::B_Check);
        d->m_Time->setStatus(BmpButton::B_Normal);
        d->m_Version->setStatus(BmpButton::B_Normal);
        break;
    }
    case CB_Time: {
        d->m_Display->setStatus(BmpButton::B_Normal);
        d->m_Volume->setStatus(BmpButton::B_Normal);
        d->m_Language->setStatus(BmpButton::B_Normal);
        d->m_Time->setStatus(BmpButton::B_Check);
        d->m_Version->setStatus(BmpButton::B_Normal);
        break;
    }
    case CB_Version: {
        d->m_Display->setStatus(BmpButton::B_Normal);
        d->m_Volume->setStatus(BmpButton::B_Normal);
        d->m_Language->setStatus(BmpButton::B_Normal);
        d->m_Time->setStatus(BmpButton::B_Normal);
        d->m_Version->setStatus(BmpButton::B_Check);
        break;
    }
    default: {
        break;
    }
    }
}
void SettingGeneralToolWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(0, 0, 180, 334, this);
}

void SettingGeneralToolWidget::onToolButtonRelease()
{
    Q_D(SettingGeneralToolWidget);
    BmpButton* ptr = static_cast<BmpButton*>(sender());
    if (ptr == d->m_Display) {
        toolButtonRelease(SettingGeneralToolWidget::CB_Dispaly);
    } else if (ptr == d->m_Volume) {
        toolButtonRelease(SettingGeneralToolWidget::CB_Volume);
    } else if (ptr == d->m_Language) {
        toolButtonRelease(SettingGeneralToolWidget::CB_Language);
    } else if (ptr == d->m_Time) {
        toolButtonRelease(SettingGeneralToolWidget::CB_Time);
    } else if (ptr == d->m_Version) {
        toolButtonRelease(SettingGeneralToolWidget::CB_Version);
    }
}

SettingGeneralToolWidgetPrivate::SettingGeneralToolWidgetPrivate(SettingGeneralToolWidget *parent)
    : q_ptr(parent)
{
    m_Display = NULL;
    m_Volume = NULL;
    m_Language = NULL;
    m_Time = NULL;
    m_Version = NULL;
    initializeBasicWidget();
    initializeParent();
}

SettingGeneralToolWidgetPrivate::~SettingGeneralToolWidgetPrivate()
{
}

void SettingGeneralToolWidgetPrivate::initializeParent()
{
    Q_Q(SettingGeneralToolWidget);
    q->setVisible(true);
}

void SettingGeneralToolWidgetPrivate::initializeBasicWidget()
{
    Q_Q(SettingGeneralToolWidget);
    Qt::ConnectionType type = static_cast<Qt::ConnectionType>(Qt::UniqueConnection | Qt::AutoConnection);
    m_Display = new BmpButton(q);
    m_Display->setNormalBmpPath(QString(":/Images/SettingGeneralDisplayNormal.png"));
    m_Display->setPressBmpPath(QString(":/Images/SettingGeneralDisplayPress.png"));
    m_Display->setCheckBmpPath(QString(":/Images/SettingGeneralDisplayPress.png"));
    g_Widget->geometryFit(24, 0, 138, 66, m_Display);
    m_Display->setVisible(true);
    QObject::connect(m_Display, ARKSENDER(released()),
                     q,         ARKRECEIVER(onToolButtonRelease()),
                     type);
    m_Volume = new BmpButton(q);
    m_Volume->setNormalBmpPath(QString(":/Images/SettingGeneralVolumeNormal.png"));
    m_Volume->setPressBmpPath(QString(":/Images/SettingGeneralVolumePress.png"));
    m_Volume->setCheckBmpPath(QString(":/Images/SettingGeneralVolumePress.png"));
    g_Widget->geometryFit(24, 67, 138, 66, m_Volume);
    m_Volume->setVisible(true);
    QObject::connect(m_Volume, ARKSENDER(released()),
                     q,        ARKRECEIVER(onToolButtonRelease()),
                     type);
    m_Language = new BmpButton(q);
    m_Language->setNormalBmpPath(QString(":/Images/SettingGeneralLanguageNormal.png"));
    m_Language->setPressBmpPath(QString(":/Images/SettingGeneralLanguagePress.png"));
    m_Language->setCheckBmpPath(QString(":/Images/SettingGeneralLanguagePress.png"));
    g_Widget->geometryFit(24, 134, 138, 66, m_Language);
    m_Language->setVisible(true);
    QObject::connect(m_Language, ARKSENDER(released()),
                     q,          ARKRECEIVER(onToolButtonRelease()),
                     type);
    m_Time = new BmpButton(q);
    m_Time->setNormalBmpPath(QString(":/Images/SettingGeneralTimeNormal.png"));
    m_Time->setPressBmpPath(QString(":/Images/SettingGeneralTimePress.png"));
    m_Time->setCheckBmpPath(QString(":/Images/SettingGeneralTimePress.png"));
    g_Widget->geometryFit(24, 201, 138, 66, m_Time);
    m_Time->setVisible(true);
    QObject::connect(m_Time, ARKSENDER(released()),
                     q,      ARKRECEIVER(onToolButtonRelease()),
                     type);
    m_Version = new BmpButton(q);
    m_Version->setNormalBmpPath(QString(":/Images/SettingGeneralVersionNormal.png"));
    m_Version->setPressBmpPath(QString(":/Images/SettingGeneralVersionPress.png"));
    m_Version->setCheckBmpPath(QString(":/Images/SettingGeneralVersionPress.png"));
    g_Widget->geometryFit(24, 268, 138, 66, m_Version);
    m_Version->setVisible(true);
    QObject::connect(m_Version, ARKSENDER(released()),
                     q,         ARKRECEIVER(onToolButtonRelease()),
                     type);
}
