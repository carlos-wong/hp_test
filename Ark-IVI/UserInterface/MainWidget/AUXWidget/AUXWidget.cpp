#include "AUXWidget.h"
#include "AutoConnect.h"
#include "BmpWidget.h"
#include "MessageBox.h"
#include "BusinessLogic/Audio.h"
#include <QPainter>
#include <QPaintEvent>
#include <QTimer>
#include <QDebug>
#include <QFile>

namespace SourceString {
static const QString No_AUX_Signal = QString(QObject::tr("No AUX Signal..."));
}

class AUXWidgetPrivate
{
public:
    explicit AUXWidgetPrivate(AUXWidget* parent);
    ~AUXWidgetPrivate();
    void initializeParent();
    void initializeTimer();
    void initializeMessageBox();
private:
    Q_DECLARE_PUBLIC(AUXWidget)
    AUXWidget* const q_ptr;
    QTimer* m_Timer;
    MessageBox* m_MessageBox;
    QRect m_TouchRect;
    AVStatus m_AVStatus;
};

AUXWidget::AUXWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new AUXWidgetPrivate(this))
{
}

AUXWidget::~AUXWidget()
{
    if (NULL != d_ptr) {
        delete d_ptr;
    }
}

void AUXWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(0, 0, 800, 480, this);
}

void AUXWidget::mouseReleaseEvent(QMouseEvent *event)
{
    Q_D(AUXWidget);
    if (g_Widget->geometryFit(0, 52, 800, 428, NULL) == d->m_TouchRect) {
        g_Widget->setWidgetType(Widget::T_AUXFullScreen, Widget::T_AUX, WidgetStatus::Show);
    } else {
        g_Widget->setWidgetType(Widget::T_AUX, Widget::T_AUX, WidgetStatus::Show);
    }
}

void AUXWidget::paintEvent(QPaintEvent *event)
{
    Q_D(AUXWidget);
    if (AVS_NoDetact == d->m_AVStatus) {
        QPainter painter(this);
        painter.fillRect(event->rect(), Qt::black);
    }
}

void AUXWidget::onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status)
{
    Q_D(AUXWidget);
    switch (destinationType) {
    case Widget::T_Home:
    case Widget::T_Back: {
        if (WidgetStatus::RequestShow == status) {
            if (isVisible()) {
                g_Widget->setWidgetType(Widget::T_ViewPaper1, Widget::T_AUX, WidgetStatus::RequestShow);
            }
        }
        break;
    }
    case Widget::T_AUX: {
        if (WidgetStatus::RequestShow == status) {
            d->initializeMessageBox();
            d->initializeTimer();
            connectSignalAndSlotByNamesake(g_Setting, this, ARKRECEIVER(onAVStatus(const int, const int)));
            g_Setting->startAV(AVT_1, 800 * g_Widget->widthScalabilityFactor(), 480 * g_Widget->heightScalabilityFactor());
        } else if (WidgetStatus::Show == status) {
            d->m_TouchRect = g_Widget->geometryFit(0, 52, 800, 428, NULL);
            d->m_Timer->start();
            setVisible(true);
        }
        break;
    }
    case Widget::T_AUXFullScreen: {
        if (WidgetStatus::RequestShow == status) {
            g_Setting->startAV(AVT_1, 800 * g_Widget->widthScalabilityFactor(), 480 * g_Widget->heightScalabilityFactor());
        } else if (WidgetStatus::Show == status) {
            d->m_TouchRect = g_Widget->geometryFit(0, 0, 800, 480, NULL);
            d->m_Timer->stop();
            setVisible(true);
        }
        break;
    }
    default: {
        if (WidgetStatus::Show == status) {
            if (isVisible()) {
                d->m_Timer->stop();
                g_Setting->stopAV(AVT_1);
                d->m_AVStatus = AVS_NoDetact;
                repaint();
                g_Audio->releaseAudioSource(AS_Aux);
                setVisible(false);
                g_Setting->setLauncherTVOUT(true);
            }
        }
        break;
    }
    }
}

void AUXWidget::onAVStatus(const AVType type, const AVStatus status)
{
    qDebug() << __PRETTY_FUNCTION__ << type << status;
    if (AVT_1 == type) {
        QFile file(QString("/tmp/video"));
        if (!file.exists()
                && file.open(QIODevice::WriteOnly)) {
            file.close();
        }
        Q_D(AUXWidget);
        if (!isVisible()) {
            g_Widget->setWidgetType(Widget::T_AUX, Widget::T_AUX, WidgetStatus::Show);
        }
        d->m_AVStatus = status;
        if (AVS_NoDetact == d->m_AVStatus) {
            g_Audio->releaseAudioSource(AS_Aux);
            d->m_MessageBox->setVisible(true);
        } else {
            g_Audio->requestAudioSource(AS_Aux);
            d->m_MessageBox->setVisible(false);
        }
        g_Setting->setLauncherTVOUT(AVS_NoDetact == d->m_AVStatus);
    }
}

void AUXWidget::onTimeout()
{
    Q_D(AUXWidget);
    if (g_Widget->geometryFit(0, 52, 800, 428, NULL) == d->m_TouchRect) {
        g_Widget->setWidgetType(Widget::T_AUXFullScreen, Widget::T_AUX, WidgetStatus::Show);
    }
}

AUXWidgetPrivate::AUXWidgetPrivate(AUXWidget *parent)
    : q_ptr(parent)
{
    m_Timer = NULL;
    m_MessageBox = NULL;
    m_TouchRect = QRect(0, 52, 800, 428);
    m_AVStatus = AVS_NoDetact;
    initializeParent();
}

AUXWidgetPrivate::~AUXWidgetPrivate()
{
}

void AUXWidgetPrivate::initializeParent()
{
    Q_Q(AUXWidget);
    q->setVisible(false);
    connectSignalAndSlotByNamesake(g_Widget, q, ARKRECEIVER(onWidgetTypeChange(const Widget::Type, const Widget::Type, const QString &)));
}

void AUXWidgetPrivate::initializeTimer()
{
    if (NULL == m_Timer) {
        Q_Q(AUXWidget);
        m_Timer = new QTimer(q);
        m_Timer->setSingleShot(true);
        m_Timer->setInterval(5000);
        QObject::connect(m_Timer, ARKSENDER(timeout()),
                         q,       ARKRECEIVER(onTimeout()));
    }
}

void AUXWidgetPrivate::initializeMessageBox()
{
    if (NULL == m_MessageBox) {
        Q_Q(AUXWidget);
        m_MessageBox = new MessageBox(q);
        m_MessageBox->setVisible(false);
        m_MessageBox->setBackgroundBmpPath(QString(":/Images/MessageBoxBackground.png"));
        m_MessageBox->setText(SourceString::No_AUX_Signal);
        m_MessageBox->setAutoHide(false);
        g_Widget->geometryFit(0, 0, 800, 480, m_MessageBox);
    }
}

