#include "SettingSoundSoundWidget.h"
#include "AutoConnect.h"
#include "BmpWidget.h"
#include "BmpButton.h"
#include "TextWidget.h"
#include "BusinessLogic/Widget.h"
#include <QPaintEvent>
#include <QPainter>
#include <QDebug>
#include <QMouseEvent>

namespace SourceString {
static const QString Master(QObject::tr("Master"));
static const QString Slave(QObject::tr("Slave"));
static const QString Rear_Left(QObject::tr("Rear Left"));
static const QString Rear_Right(QObject::tr("Rear Right"));
static const QString Custom(QObject::tr("Custom"));
}

class SettingSoundSoundWidgetPrivate
{
public:
    explicit SettingSoundSoundWidgetPrivate(SettingSoundSoundWidget* parent);
    ~SettingSoundSoundWidgetPrivate();
    void initializeParent();
    void initializeBasicWidget();
    void initializeTimer();
    void connectAllSlots();
    void setPosition(const short x, const short y);
    void setSoundItemBtn(const int item);
private:
    Q_DECLARE_PUBLIC(SettingSoundSoundWidget)
    SettingSoundSoundWidget* const q_ptr;
    BmpWidget* m_Background;
    BmpWidget* m_Ticks;
    BmpButton* m_MasterBtn;
    BmpButton* m_SlaveBtn;
    BmpButton* m_RearLeftBtn;
    BmpButton* m_RearRightBtn;
    BmpButton* m_CustomBtn;
    bool m_Filter;
    short m_X;
    short m_Y;
    QTimer* m_Timer;
};

SettingSoundSoundWidget::SettingSoundSoundWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new SettingSoundSoundWidgetPrivate(this))
{
}

SettingSoundSoundWidget::~SettingSoundSoundWidget()
{
    if (NULL != d_ptr) {
        delete d_ptr;
    }
}

void SettingSoundSoundWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(180, 0, 600, 334, this);
}

void SettingSoundSoundWidget::mousePressEvent(QMouseEvent *event)
{
    Q_D(SettingSoundSoundWidget);
    if (d->m_Background->geometry().contains(event->pos())) {
        d->m_Filter = true;
        short xPos = event->pos().x() - d->m_Background->x() - 42 * g_Widget->widthScalabilityFactor();
        xPos = (xPos + 6.428571f) / ((12.857142f * g_Widget->widthScalabilityFactor()));
        if (xPos < 0) {
            xPos = 0;
        } else if (xPos > 14) {
            xPos = 14;
        }
        xPos -= 7;
        short yPos = event->pos().y() - d->m_Background->y() - 37 * g_Widget->heightScalabilityFactor();
        yPos = (yPos + 6.535714f) / (13.071428f * g_Widget->heightScalabilityFactor());
        if (yPos < 0) {
            yPos = 0;
        } else if (yPos > 14) {
            yPos = 14;
        }
        yPos -= 7;
        d->setSoundItemBtn(SI_Custom);
        d->setPosition(xPos, yPos);
        d->initializeTimer();
        d->m_Timer->start();
    }
}

void SettingSoundSoundWidget::mouseMoveEvent(QMouseEvent *event)
{
    Q_D(SettingSoundSoundWidget);
    if (d->m_Background->geometry().contains(event->pos())) {
        short xPos = event->pos().x() - d->m_Background->x() - 42 * g_Widget->widthScalabilityFactor();
        xPos = (xPos + 6.428571f) / ((12.857142f * g_Widget->widthScalabilityFactor()));
        if (xPos < 0) {
            xPos = 0;
        } else if (xPos > 14) {
            xPos = 14;
        }
        xPos -= 7;
        short yPos = event->pos().y() - d->m_Background->y() - 37 * g_Widget->heightScalabilityFactor();
        yPos = (yPos + 6.535714f) / (13.071428f * g_Widget->heightScalabilityFactor());
        if (yPos < 0) {
            yPos = 0;
        } else if (yPos > 14) {
            yPos = 14;
        }
        yPos -= 7;
        d->setPosition(xPos, yPos);
        d->initializeTimer();
        d->m_Timer->start();
    }
}

void SettingSoundSoundWidget::mouseReleaseEvent(QMouseEvent *event)
{
    Q_D(SettingSoundSoundWidget);
    if (d->m_Background->geometry().contains(event->pos())) {
        d->m_Filter = false;
        d->initializeTimer();
        d->m_Timer->start();
    }
}

void SettingSoundSoundWidget::onSoundItemChange(const int item, const int left, const int right)
{
    Q_D(SettingSoundSoundWidget);
    d->setSoundItemBtn(item);
    if (!d->m_Filter) {
        d->setPosition(left, right);
    }
}

void SettingSoundSoundWidget::onTimeout()
{
    Q_D(SettingSoundSoundWidget);
    g_Audio->setSoundItem(SI_Custom, d->m_X, d->m_Y);
}

void SettingSoundSoundWidget::onToolButtonRelease()
{
    BmpButton* ptr = static_cast<BmpButton*>(sender());
    Q_D(SettingSoundSoundWidget);
    if (ptr == d->m_MasterBtn) {
        g_Audio->setSoundItem(SI_Master, -8, -8);
    } else if (ptr == d->m_SlaveBtn) {
        g_Audio->setSoundItem(SI_Slave, -8, -8);
    } else if (ptr == d->m_RearLeftBtn) {
        g_Audio->setSoundItem(SI_RearLeft, -8, -8);
    } else if (ptr == d->m_RearRightBtn) {
        g_Audio->setSoundItem(SI_RearRight, -8, -8);
    } else if (ptr == d->m_CustomBtn) {
        g_Audio->setSoundItem(SI_Custom, -8, -8);
    }
}

SettingSoundSoundWidgetPrivate::SettingSoundSoundWidgetPrivate(SettingSoundSoundWidget *parent)
    : q_ptr(parent)
{
    m_Background = NULL;
    m_Ticks = NULL;
    m_MasterBtn = NULL;
    m_SlaveBtn = NULL;
    m_RearLeftBtn = NULL;
    m_RearRightBtn = NULL;
    m_CustomBtn = NULL;
    m_Filter = false;
    m_X = 0;
    m_Y = 0;
    m_Timer = NULL;
    initializeParent();
    initializeBasicWidget();
    connectAllSlots();
}

SettingSoundSoundWidgetPrivate::~SettingSoundSoundWidgetPrivate()
{
}

void SettingSoundSoundWidgetPrivate::initializeParent()
{
    Q_Q(SettingSoundSoundWidget);
    q->setVisible(false);
}

void SettingSoundSoundWidgetPrivate::initializeBasicWidget()
{
    Q_Q(SettingSoundSoundWidget);
    m_Background = new BmpWidget(q);
    m_Background->setBackgroundBmpPath(QString(":/Images/SettingSoundSoundBackground.png"));
    g_Widget->geometryFit(56, 39, 266, 250, m_Background);
    qDebug() << m_Background->geometry();
    m_Background->setVisible(true);

    m_Ticks = new BmpWidget(q);
    m_Ticks->setBackgroundBmpPath(QString(":/Images/SettingSoundSoundTicks.png"));
    setPosition(0, 0);
    m_Ticks->setVisible(true);

    Qt::ConnectionType type = static_cast<Qt::ConnectionType>(Qt::UniqueConnection | Qt::AutoConnection);
    m_MasterBtn = new BmpButton(q);
    m_MasterBtn->setNormalBmpPath(QString(":/Images/SettingSoundSoundBlankNormal.png"));
    m_MasterBtn->setPressBmpPath(QString(":/Images/SettingSoundSoundBlankPress.png"));
    m_MasterBtn->setCheckBmpPath(QString(":/Images/SettingSoundSoundBlankPress.png"));
    g_Widget->geometryFit(394, 7 + (7 + 58) * 0, 170, 58, m_MasterBtn);
    m_MasterBtn->setText(SourceString::Master);
    m_MasterBtn->setLanguageType(BmpButton::T_Translate);
    m_MasterBtn->setVisible(true);
    QObject::connect(m_MasterBtn, ARKSENDER(released()),
                     q,           ARKRECEIVER(onToolButtonRelease()),
                     type);

    m_SlaveBtn = new BmpButton(q);
    m_SlaveBtn->setNormalBmpPath(QString(":/Images/SettingSoundSoundBlankNormal.png"));
    m_SlaveBtn->setPressBmpPath(QString(":/Images/SettingSoundSoundBlankPress.png"));
    m_SlaveBtn->setCheckBmpPath(QString(":/Images/SettingSoundSoundBlankPress.png"));
    g_Widget->geometryFit(394, 7 + (7 + 58) * 1, 170, 58, m_SlaveBtn);
    m_SlaveBtn->setText(SourceString::Slave);
    m_SlaveBtn->setLanguageType(BmpButton::T_Translate);
    m_SlaveBtn->setVisible(true);
    QObject::connect(m_SlaveBtn, ARKSENDER(released()),
                     q,          ARKRECEIVER(onToolButtonRelease()),
                     type);

    m_RearLeftBtn = new BmpButton(q);
    m_RearLeftBtn->setNormalBmpPath(QString(":/Images/SettingSoundSoundBlankNormal.png"));
    m_RearLeftBtn->setPressBmpPath(QString(":/Images/SettingSoundSoundBlankPress.png"));
    m_RearLeftBtn->setCheckBmpPath(QString(":/Images/SettingSoundSoundBlankPress.png"));
    g_Widget->geometryFit(394, 7 + (7 + 58) * 2, 170, 58, m_RearLeftBtn);
    m_RearLeftBtn->setText(SourceString::Rear_Left);
    m_RearLeftBtn->setLanguageType(BmpButton::T_Translate);
    m_RearLeftBtn->setVisible(true);
    QObject::connect(m_RearLeftBtn, ARKSENDER(released()),
                     q,             ARKRECEIVER(onToolButtonRelease()),
                     type);

    m_RearRightBtn = new BmpButton(q);
    m_RearRightBtn->setNormalBmpPath(QString(":/Images/SettingSoundSoundBlankNormal.png"));
    m_RearRightBtn->setPressBmpPath(QString(":/Images/SettingSoundSoundBlankPress.png"));
    m_RearRightBtn->setCheckBmpPath(QString(":/Images/SettingSoundSoundBlankPress.png"));
    g_Widget->geometryFit(394, 7 + (7 + 58) * 3, 170, 58, m_RearRightBtn);
    m_RearRightBtn->setText(SourceString::Rear_Right);
    m_RearRightBtn->setLanguageType(BmpButton::T_Translate);
    m_RearRightBtn->setVisible(true);
    QObject::connect(m_RearRightBtn, ARKSENDER(released()),
                     q,              ARKRECEIVER(onToolButtonRelease()),
                     type);

    m_CustomBtn = new BmpButton(q);
    m_CustomBtn->setNormalBmpPath(QString(":/Images/SettingSoundSoundBlankNormal.png"));
    m_CustomBtn->setPressBmpPath(QString(":/Images/SettingSoundSoundBlankPress.png"));
    m_CustomBtn->setCheckBmpPath(QString(":/Images/SettingSoundSoundBlankPress.png"));
    g_Widget->geometryFit(394, 7 + (7 + 58) * 4, 170, 58, m_CustomBtn);
    m_CustomBtn->setText(SourceString::Custom);
    m_CustomBtn->setLanguageType(BmpButton::T_Translate);
    m_CustomBtn->setVisible(true);
    QObject::connect(m_CustomBtn, ARKSENDER(released()),
                     q,            ARKRECEIVER(onToolButtonRelease()),
                     type);

    SoundItem item = AudioPersistent::getSoundItem();
    setSoundItemBtn(item);
    int left;
    int right;
    switch (item) {
    case SI_Master: {
        left = -3;
        right = -3;
        break;
    }
    case SI_Slave: {
        left = 3;
        right = -3;
        break;
    }
    case SI_RearLeft: {
        left = -3;
        right = 3;
        break;
    }
    case SI_RearRight: {
        left = 3;
        right = 3;
        break;
    }
    default: {
        left = AudioPersistent::getLeftSound();
        right = AudioPersistent::getRightSound();
        break;
    }
    }
    setPosition(left, right);
}

void SettingSoundSoundWidgetPrivate::initializeTimer()
{
    Q_Q(SettingSoundSoundWidget);
    if (NULL == m_Timer) {
        m_Timer = new QTimer(q);
        m_Timer->setInterval(1);
        m_Timer->setSingleShot(true);
        QObject::connect(m_Timer, ARKSENDER(timeout()),
                         q,       ARKRECEIVER(onTimeout()));
    }
}

void SettingSoundSoundWidgetPrivate::connectAllSlots()
{
    Q_Q(SettingSoundSoundWidget);
    QObject::connect(g_Audio, ARKSENDER(onSoundItemChange(const int, const int, const int)),
                     q,       ARKRECEIVER(onSoundItemChange(const int, const int, const int)),
                     Qt::UniqueConnection);
}

void SettingSoundSoundWidgetPrivate::setPosition(const short x, const short y)
{
    m_X = x;
    m_Y = y;
    short xPos = 164 + x * 12.857142f;
    short yPos = 136 + y * 13.071428f;
    g_Widget->geometryFit(xPos, yPos, 50, 54, m_Ticks);
}

void SettingSoundSoundWidgetPrivate::setSoundItemBtn(const int item)
{
    m_MasterBtn->setStatus(item == SI_Master? BmpButton::B_Check: BmpButton::B_Normal);
    m_SlaveBtn->setStatus(item == SI_Slave? BmpButton::B_Check: BmpButton::B_Normal);
    m_RearLeftBtn->setStatus(item == SI_RearLeft? BmpButton::B_Check: BmpButton::B_Normal);
    m_RearRightBtn->setStatus(item == SI_RearRight? BmpButton::B_Check: BmpButton::B_Normal);
    m_CustomBtn->setStatus(item == SI_Custom? BmpButton::B_Check: BmpButton::B_Normal);
}
