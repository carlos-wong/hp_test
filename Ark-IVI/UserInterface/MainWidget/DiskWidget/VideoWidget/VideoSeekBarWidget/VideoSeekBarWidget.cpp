#include "VideoSeekBarWidget.h"
#include "BusinessLogic/Widget.h"
#include "TextWidget.h"
#include "Slider.h"
#include "AutoConnect.h"
#include <QDebug>

class VideoSeekBarWidgetPrivate
{
public:
    VideoSeekBarWidgetPrivate(VideoSeekBarWidget* parent);
    ~VideoSeekBarWidgetPrivate();
    void initializeParent();
    void initializeBasic();
    void connectAllSlots();
    QString convertTime(const int time);
private:
    Q_DECLARE_PUBLIC(VideoSeekBarWidget)
    VideoSeekBarWidget* const q_ptr;
    TextWidget* m_ElapsedTimeText;
    TextWidget* m_EndTimeText;
    Slider* m_Slider;
};

VideoSeekBarWidget::VideoSeekBarWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new VideoSeekBarWidgetPrivate(this))
{
}

VideoSeekBarWidget::~VideoSeekBarWidget()
{
    if (NULL != d_ptr) {
        delete d_ptr;
    }
}

void VideoSeekBarWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(0, 332, 800, 60, this);
}

void VideoSeekBarWidget::onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status)
{
    switch (destinationType) {
    case Widget::T_SDVideo:
    case Widget::T_USBVideo: {
        if (WidgetStatus::Show == status) {
            setVisible(true);
        }
        break;
    }
    case Widget::T_SDVideoFullScreen:
    case Widget::T_USBVideoFullScreen: {
        if (WidgetStatus::Show == status) {
            setVisible(false);
        }
        break;
    }
    default : {
        break;
    }
    }
}

void VideoSeekBarWidget::onVideoPlayerInformation(const int type, const int index, const QString &fileName, const int endTime)
{
    Q_D(VideoSeekBarWidget);
    d->m_EndTimeText->setText(d->convertTime(endTime));
    d->m_Slider->setDisabled(0 == endTime);
    d->m_Slider->setRange(0, endTime);
}

void VideoSeekBarWidget::onVideoPlayerElapsedInformation(const int elapsedTime, const int elapsedMillesimal)
{
    Q_D(VideoSeekBarWidget);
    d->m_ElapsedTimeText->setText(d->convertTime(elapsedTime));
    if (!QFile::exists(QString("/tmp/seek"))) {
        if (!d->m_Slider->isSliderDown()) {
            d->m_Slider->setValue(elapsedTime);
        }
    }
}

void VideoSeekBarWidget::onSliderReleased(const int value)
{
    Q_D(VideoSeekBarWidget);
    if (0 != d->m_Slider->maximum()) {
        int millesimal = 1000 * (static_cast<float>(value) / d->m_Slider->maximum());
        g_Multimedia->videoPlayerSeekToMillesimal(millesimal);
    }
}

VideoSeekBarWidgetPrivate::VideoSeekBarWidgetPrivate(VideoSeekBarWidget *parent)
    : q_ptr(parent)
{
    m_ElapsedTimeText = NULL;
    m_EndTimeText = NULL;
    m_Slider = NULL;
    initializeBasic();
    initializeParent();
    connectAllSlots();
}

VideoSeekBarWidgetPrivate::~VideoSeekBarWidgetPrivate()
{
}

void VideoSeekBarWidgetPrivate::initializeParent()
{
    Q_Q(VideoSeekBarWidget);
    q->setVisible(true);
}

void VideoSeekBarWidgetPrivate::initializeBasic()
{
    Q_Q(VideoSeekBarWidget);
    m_ElapsedTimeText = new TextWidget(q);
    m_ElapsedTimeText->setAlignmentFlag(Qt::AlignLeft | Qt::AlignVCenter);
    m_ElapsedTimeText->setText(QString("00:00:00"));
    g_Widget->geometryFit(0, 0, 100, 30, m_ElapsedTimeText);
    m_ElapsedTimeText->setVisible(true);
    m_EndTimeText = new TextWidget(q);
    m_EndTimeText->setText(QString("00:00:00"));
    g_Widget->geometryFit(700, 0, 100, 30, m_EndTimeText);
    m_EndTimeText->setAlignmentFlag(Qt::AlignRight | Qt::AlignVCenter);
    m_EndTimeText->setVisible(true);
    m_Slider = new Slider(q);
    g_Widget->geometryFit(0, 30, 800, 30, m_Slider);
    m_Slider->setStyleSheet(QString("QSlider::groove:horizontal {"
                                    "height: " + QString::number(int(30 * g_Widget->heightScalabilityFactor())) + "px;"
                                                                                                                  "background: transparent;"
                                                                                                                  "}"
                                                                                                                  "QSlider::handle:horizontal {"
                                                                                                                  "width: " + QString::number(int(26 * g_Widget->widthScalabilityFactor())) + "px;"
                                                                                                                                                                                              "image: url(:/Images/MultimediaSeekBaTickMarks.png);"
                                                                                                                                                                                              "}"
                                                                                                                                                                                              "QSlider::sub-page:horizontal {"
                                                                                                                                                                                              "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #003e61, stop: 1 #009ffa); "
                                                                                                                                                                                              "margin: " + QString::number(int(10 * g_Widget->widthScalabilityFactor())) + "px 0px " + QString::number(int(10 * g_Widget->widthScalabilityFactor())) + "px 0px;"
                                                                                                                                                                                                                                                                                                                                                       "border-radius: " + QString::number(int(5 * g_Widget->widthScalabilityFactor())) + "px;"
                                                                                                                                                                                                                                                                                                                                                                                                                                          "}"
                                                                                                                                                                                                                                                                                                                                                                                                                                          "QSlider::add-page:horizontal {"
                                                                                                                                                                                                                                                                                                                                                                                                                                          "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #616161, stop: 1 #2b2b2b); "
                                                                                                                                                                                                                                                                                                                                                                                                                                          "margin: " + QString::number(int(10 * g_Widget->widthScalabilityFactor())) + "px 0px " + QString::number(int(10 * g_Widget->widthScalabilityFactor())) + "px 0px;"
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   "border-radius: " + QString::number(int(5 * g_Widget->widthScalabilityFactor())) + "px;"
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      "}"));
    m_Slider->setVisible(true);
}

void VideoSeekBarWidgetPrivate::connectAllSlots()
{
    Q_Q(VideoSeekBarWidget);
    connectSignalAndSlotByNamesake(g_Widget, q, ARKRECEIVER(onWidgetTypeChange(const Widget::Type, const Widget::Type, const QString &)));
    connectSignalAndSlotByNamesake(g_Multimedia, q, ARKRECEIVER(onVideoPlayerInformation(const int, const int, const QString &, const int)));
    connectSignalAndSlotByNamesake(g_Multimedia, q, ARKRECEIVER(onVideoPlayerElapsedInformation(const int, const int)));
    Qt::ConnectionType type = static_cast<Qt::ConnectionType>(Qt::AutoConnection | Qt::UniqueConnection);
    QObject::connect(m_Slider, ARKSENDER(sliderReleased(const int)),
                     q,        ARKRECEIVER(onSliderReleased(const int)),
                     type);
}

QString VideoSeekBarWidgetPrivate::convertTime(const int time)
{
    QString hour("%1");
    QString minute("%1");
    QString second("%1");
    return hour.arg((time / 60) / 60, 2, 10, QChar('0'))
            + QString(":") + minute.arg((time / 60) % 60, 2, 10, QChar('0'))
            + QString(":") + second.arg(time % 60, 2, 10, QChar('0'));
}
