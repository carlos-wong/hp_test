#include "MusicSeekBarWidget.h"
#include "BusinessLogic/Widget.h"
#include "TextWidget.h"
#include "AutoConnect.h"
#include "Slider.h"
#include <QPainter>
#include <QPixmap>
#include <QDebug>

class MusicSeekBarWidgetPrivate
{
public:
    MusicSeekBarWidgetPrivate(MusicSeekBarWidget* parent);
    ~MusicSeekBarWidgetPrivate();
    void initializeParent();
    void initializeBasic();
    void connectAllSlots();
    QString convertTime(const int time);
private:
    Q_DECLARE_PUBLIC(MusicSeekBarWidget)
    MusicSeekBarWidget* const q_ptr;
    TextWidget* m_ElapsedTimeText;
    TextWidget* m_EndTimeText;
    Slider* m_Slider;
};

MusicSeekBarWidget::MusicSeekBarWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new MusicSeekBarWidgetPrivate(this))
{
}

MusicSeekBarWidget::~MusicSeekBarWidget()
{
    if (NULL != d_ptr) {
        delete d_ptr;
    }
}

void MusicSeekBarWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(66, 318, 668, 60, this);
}

void MusicSeekBarWidget::onMusicPlayerElapsedInformation(const int elapsedTime, const int elapsedMillesimal)
{
    Q_D(MusicSeekBarWidget);
    d->m_ElapsedTimeText->setText(d->convertTime(elapsedTime));
    if (!QFile::exists(QString("/tmp/seek"))) {
        if (!d->m_Slider->isSliderDown()) {
            d->m_Slider->setValue(elapsedTime);
        }
    }
}

void MusicSeekBarWidget::onMusicPlayerID3TagChange(const int type, const int index, const QString &fileName, const QString& title, const QString& artist, const QString& album, const int endTime)
{
    Q_D(MusicSeekBarWidget);
    d->m_EndTimeText->setText(d->convertTime(endTime));
    d->m_Slider->setDisabled(0 == endTime);
    d->m_Slider->setRange(0, endTime);
}

void MusicSeekBarWidget::onMusicPlayerPlayStatus(const DeviceWatcherType type, const MusicPlayerPlayStatus status)
{
    Q_D(MusicSeekBarWidget);
    d->m_Slider->setEnabled(MPPS_Start != status);
}

void MusicSeekBarWidget::onSliderReleased(const int value)
{
    Q_D(MusicSeekBarWidget);
    if (0 != d->m_Slider->maximum()) {
        int millesimal = 1000 * (static_cast<float>(value) / d->m_Slider->maximum());
        g_Multimedia->musicPlayerSeekToMillesimal(millesimal);
    }
}

MusicSeekBarWidgetPrivate::MusicSeekBarWidgetPrivate(MusicSeekBarWidget *parent)
    : q_ptr(parent)
{
    m_ElapsedTimeText = NULL;
    m_EndTimeText = NULL;
    m_Slider = NULL;
    initializeBasic();
    initializeParent();
    connectAllSlots();
}

MusicSeekBarWidgetPrivate::~MusicSeekBarWidgetPrivate()
{
}

void MusicSeekBarWidgetPrivate::initializeParent()
{
    Q_Q(MusicSeekBarWidget);
    q->setVisible(true);
}

void MusicSeekBarWidgetPrivate::initializeBasic()
{
    Q_Q(MusicSeekBarWidget);
    m_ElapsedTimeText = new TextWidget(q);
    m_ElapsedTimeText->setAlignmentFlag(Qt::AlignLeft | Qt::AlignVCenter);
    m_ElapsedTimeText->setText(QString("00:00:00"));
    g_Widget->geometryFit(0, 0, 334, 30, m_ElapsedTimeText);
    m_ElapsedTimeText->setVisible(true);
    m_EndTimeText = new TextWidget(q);
    m_EndTimeText->setText(QString("00:00:01"));
    g_Widget->geometryFit(334, 0, 334, 30, m_EndTimeText);
    m_EndTimeText->setAlignmentFlag(Qt::AlignRight | Qt::AlignVCenter);
    m_EndTimeText->setVisible(true);
    m_Slider = new Slider(q);
    g_Widget->geometryFit(0, 30, 668, 30, m_Slider);
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

void MusicSeekBarWidgetPrivate::connectAllSlots()
{
    Q_Q(MusicSeekBarWidget);
    connectSignalAndSlotByNamesake(g_Multimedia, q, ARKRECEIVER(onMusicPlayerElapsedInformation(const int, const int)));
    connectSignalAndSlotByNamesake(g_Multimedia, q, ARKRECEIVER(onMusicPlayerID3TagChange(const int, const int, const QString &, const QString&, const QString&, const QString&, const int)));
    connectSignalAndSlotByNamesake(g_Multimedia, q, ARKRECEIVER(onMusicPlayerPlayStatus(const int, const int)));
    Qt::ConnectionType type = static_cast<Qt::ConnectionType>(Qt::UniqueConnection | Qt::AutoConnection);
    QObject::connect(m_Slider, ARKSENDER(sliderReleased(const int)),
                     q,        ARKRECEIVER(onSliderReleased(const int)),
                     type);
}

QString MusicSeekBarWidgetPrivate::convertTime(const int time)
{
    QString hour("%1");
    QString minute("%1");
    QString second("%1");
    return hour.arg((time / 60) / 60, 2, 10, QChar('0'))
            + QString(":") + minute.arg((time / 60) % 60, 2, 10, QChar('0'))
            + QString(":") + second.arg(time % 60, 2, 10, QChar('0'));
}
