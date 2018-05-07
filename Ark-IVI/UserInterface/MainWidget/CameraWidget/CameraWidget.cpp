#include "CameraWidget.h"
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
static const QString No_Camera_Signal = QString(QObject::tr("No Camera Signal..."));
}

class CameraWidgetPrivate
{
public:
    explicit CameraWidgetPrivate(CameraWidget* parent);
    ~CameraWidgetPrivate();
    void initializeParent();
    void initializeTimer();
    void initializeMessageBox();
private:
    Q_DECLARE_PUBLIC(CameraWidget)
    CameraWidget* const q_ptr;
    QTimer* m_Timer;
    MessageBox* m_MessageBox;
    QRect m_TouchRect;
    AVStatus m_AVStatus;
};

CameraWidget::CameraWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new CameraWidgetPrivate(this))
{
}

CameraWidget::~CameraWidget()
{
    if (NULL != d_ptr) {
        delete d_ptr;
    }
}

void CameraWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(0, 0, 800, 480, this);
}

void CameraWidget::mouseReleaseEvent(QMouseEvent *event)
{
    Q_D(CameraWidget);
    if (g_Widget->geometryFit(0, 52, 800, 428, NULL) == d->m_TouchRect) {
        g_Widget->setWidgetType(Widget::T_CameraFullScreen, Widget::T_Camera, WidgetStatus::Show);
    } else {
        g_Widget->setWidgetType(Widget::T_Camera, Widget::T_Camera, WidgetStatus::Show);
    }
}

void CameraWidget::paintEvent(QPaintEvent *event)
{
    Q_D(CameraWidget);
    if (AVS_NoDetact == d->m_AVStatus) {
        QPainter painter(this);
        painter.fillRect(event->rect(), Qt::black);
    }
}

void CameraWidget::onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status)
{
    Q_D(CameraWidget);
    switch (destinationType) {
    case Widget::T_Home:
    case Widget::T_Back: {
        if (WidgetStatus::RequestShow == status) {
            if (isVisible()) {
                g_Widget->setWidgetType(Widget::T_ViewPaper1, Widget::T_Camera, WidgetStatus::RequestShow);
            }
        }
        break;
    }
    case Widget::T_Camera: {
        if (WidgetStatus::RequestShow == status) {
            d->initializeMessageBox();
            d->initializeTimer();
            connectSignalAndSlotByNamesake(g_Setting, this, ARKRECEIVER(onAVStatus(const int, const int)));
            //            g_Audio->requestAudioSource(AS_Aux);
            g_Setting->startAV(AVT_2, 800 * g_Widget->widthScalabilityFactor(), 480 * g_Widget->heightScalabilityFactor());
        } else if (WidgetStatus::Show == status) {
            d->m_TouchRect = g_Widget->geometryFit(0, 52, 800, 428, NULL);
            d->m_Timer->stop();
            setVisible(true);
        }
        break;
    }
    case Widget::T_CameraFullScreen: {
        if (WidgetStatus::RequestShow == status) {
            g_Setting->startAV(AVT_2, 800 * g_Widget->widthScalabilityFactor(), 480 * g_Widget->heightScalabilityFactor());
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
                g_Setting->stopAV(AVT_2);
                d->m_AVStatus = AVS_NoDetact;
                repaint();
                //                if (ACS_ReversingOff == g_Setting->getReversing()) {
                //                    if (QFile::exists(QString("/tmp/video"))) {
                //                        qDebug() << __PRETTY_FUNCTION__ << __LINE__;
                //                        QFile::remove(QString("/tmp/video"));
                //                    }
                //                }
                //                g_Audio->releaseAudioSource(AS_Aux);
                setVisible(false);
                g_Setting->setLauncherTVOUT(true);
            }
        }
        break;
    }
    }
}

void CameraWidget::onAVStatus(const AVType type, const AVStatus status)
{
    qDebug() << __PRETTY_FUNCTION__ << type << status;
    if (AVT_2 == type) {
        Q_D(CameraWidget);
        if (!isVisible()) {
            g_Widget->setWidgetType(Widget::T_Camera, Widget::T_Camera, WidgetStatus::Show);
        }
        d->m_AVStatus = status;
        if (AVS_NoDetact == d->m_AVStatus) {
            d->m_MessageBox->setVisible(true);
        } else {
            d->m_MessageBox->setVisible(false);
        }
        g_Setting->setLauncherTVOUT((AVS_NoDetact == d->m_AVStatus));
    }
}

void CameraWidget::onTimeout()
{
    Q_D(CameraWidget);
    if (g_Widget->geometryFit(0, 52, 800, 428, NULL) == d->m_TouchRect) {
        g_Widget->setWidgetType(Widget::T_CameraFullScreen, Widget::T_Camera, WidgetStatus::Show);
    }
}

CameraWidgetPrivate::CameraWidgetPrivate(CameraWidget *parent)
    : q_ptr(parent)
{
    m_Timer = NULL;
    m_MessageBox = NULL;
    m_TouchRect = QRect(0, 52, 800, 428);
    m_AVStatus = AVS_NoDetact;
    initializeParent();
}

CameraWidgetPrivate::~CameraWidgetPrivate()
{
}

void CameraWidgetPrivate::initializeParent()
{
    Q_Q(CameraWidget);
    q->setVisible(false);
    connectSignalAndSlotByNamesake(g_Widget, q, ARKRECEIVER(onWidgetTypeChange(const Widget::Type, const Widget::Type, const QString &)));
}

void CameraWidgetPrivate::initializeTimer()
{
    if (NULL == m_Timer) {
        Q_Q(CameraWidget);
        m_Timer = new QTimer(q);
        m_Timer->setSingleShot(true);
        m_Timer->setInterval(5000);
        QObject::connect(m_Timer, ARKSENDER(timeout()),
                         q,       ARKRECEIVER(onTimeout()));
    }
}

void CameraWidgetPrivate::initializeMessageBox()
{
    if (NULL == m_MessageBox) {
        Q_Q(CameraWidget);
        m_MessageBox = new MessageBox(q);
        m_MessageBox->setVisible(false);
        m_MessageBox->setBackgroundBmpPath(QString(":/Images/MessageBoxBackground.png"));
        m_MessageBox->setText(SourceString::No_Camera_Signal);
        m_MessageBox->setAutoHide(false);
        g_Widget->geometryFit(0, 0, 800, 480, m_MessageBox);
    }
}

