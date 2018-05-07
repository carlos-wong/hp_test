#include "RadioSelectorWidget.h"
#include "AutoConnect.h"
#include "BmpWidget.h"
#include "Slider.h"
#include <QDebug>
#include <QTimer>
#include <QMouseEvent>

class RadioSelectorWidgetPrivate
{
public:
    explicit RadioSelectorWidgetPrivate(RadioSelectorWidget* parent);
    ~RadioSelectorWidgetPrivate();
    void initialize();
    void initializeParent();
    void initializeTimer();
    void connectAllSlots();
private:
    Q_DECLARE_PUBLIC(RadioSelectorWidget)
    RadioSelectorWidget* const q_ptr;
    BmpWidget* m_Background;
    Slider* m_Slider;
    QTimer* m_Timer;
};

RadioSelectorWidget::RadioSelectorWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new RadioSelectorWidgetPrivate(this))
{
}

RadioSelectorWidget::~RadioSelectorWidget()
{
    if (NULL != d_ptr) {
        delete d_ptr;
    }
}

void RadioSelectorWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(0, 52, 800, 70, this);
}

void RadioSelectorWidget::onRadioInfoChange(const RadioBandType type, const unsigned short frequency, const unsigned short index, const unsigned short min, const unsigned short max, const unsigned short step)
{
    Q_D(RadioSelectorWidget);
    if (min != d->m_Slider->minimum()) {
        d->m_Slider->setMinimum(min);
    }
    if (max != d->m_Slider->maximum()) {
        d->m_Slider->setMaximum(max);
    }
    if (step != d->m_Slider->singleStep()) {
        d->m_Slider->setSingleStep(step);
    }
    if (!d->m_Slider->isSliderDown()) {
        d->m_Slider->setValue(frequency);
    }
}

void RadioSelectorWidget::onTimeout()
{
    Q_D(RadioSelectorWidget);
    onSliderReleased(d->m_Slider->value());
}

void RadioSelectorWidget::onSliderMoved(int value)
{
    sliderMoved(value);
    Q_D(RadioSelectorWidget);
    d->initializeTimer();
//    d->m_Timer->start();
}

void RadioSelectorWidget::onSliderReleased(int value)
{
    Q_D(RadioSelectorWidget);
    int step = g_Radio->getCurrentBandStep();
    switch (g_Radio->getCurrentBand()) {
    case RBT_FM1:
    case RBT_FM2:
    case RBT_FM3:
    case RBT_AM1:
    case RBT_AM2: {
        g_Radio->setCurrentBandFrequency((value / step) * step);
        break;
    }
    default: {
        break;
    }
    }
}

RadioSelectorWidgetPrivate::RadioSelectorWidgetPrivate(RadioSelectorWidget *parent)
    : q_ptr(parent)
{
    m_Background = NULL;
    m_Slider = NULL;
    m_Timer = NULL;
    initialize();
    initializeParent();
    connectAllSlots();
}

RadioSelectorWidgetPrivate::~RadioSelectorWidgetPrivate()
{
}

void RadioSelectorWidgetPrivate::initialize()
{
    Q_Q(RadioSelectorWidget);
    m_Background = new BmpWidget(q);
    m_Background->setBackgroundBmpPath(QString(":/Images/RadioSelectorWidgetBackground.png"));
    g_Widget->geometryFit(0, 0, 800, 60, m_Background);
    m_Background->setVisible(true);
    m_Slider = new Slider(q);
    m_Slider->setStyleSheet(QString("QSlider::groove:horizontal {"
                                    "background: transparent;"
                                    "}"
                                    "QSlider::handle:horizontal {"
                                    "width: " + QString::number(int(7 * g_Widget->widthScalabilityFactor())) + "px;"
                                                                                                               "image: url(:/Images/RadioSelectorWidgetTickMarks.png);"
                                                                                                               "}"));
    g_Widget->geometryFit(80, 0, 640, 70, m_Slider);
    QObject::connect(m_Slider, ARKSENDER(sliderMoved(int)),
                     q,        ARKRECEIVER(onSliderMoved(int)));
    QObject::connect(m_Slider, ARKSENDER(sliderReleased(int)),
                     q,        ARKRECEIVER(onSliderReleased(int)));
    m_Slider->setRange(g_Radio->getCurrentBandMin(), g_Radio->getCurrentBandMax());
    m_Slider->setSingleStep(g_Radio->getCurrentBandStep());
    m_Slider->setValue(g_Radio->getCurrendBandFrequency());
    m_Slider->setVisible(true);
}

void RadioSelectorWidgetPrivate::initializeParent()
{
    Q_Q(RadioSelectorWidget);
    q->setVisible(true);
}

void RadioSelectorWidgetPrivate::initializeTimer()
{
    Q_Q(RadioSelectorWidget);
    if (NULL == m_Timer) {
        m_Timer = new QTimer(q);
        m_Timer->setInterval(1);
        m_Timer->setSingleShot(true);
        QObject::connect(m_Timer, ARKSENDER(timeout()),
                         q,       ARKRECEIVER(onTimeout()));
    }
}

void RadioSelectorWidgetPrivate::connectAllSlots()
{
    Q_Q(RadioSelectorWidget);
    connectSignalAndSlotByNamesake(g_Radio, q, ARKRECEIVER(onRadioInfoChange(const RadioBandType , const unsigned short , const unsigned short , const unsigned short , const unsigned short , const unsigned short)));
}

