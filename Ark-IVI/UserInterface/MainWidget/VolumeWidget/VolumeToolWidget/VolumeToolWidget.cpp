#include "VolumeToolWidget.h"
#include "BmpWidget.h"
#include "BusinessLogic/Widget.h"
#include "BmpButton.h"
#include "TextWidget.h"
#include "AutoConnect.h"
#include "VolumeSliderWidget/VolumeSliderWidget.h"
#include "UserInterfaceUtility.h"

namespace SourceString {
static const QString Volume(QObject::tr("Volume"));
}

class VolumeToolWidgetPrivate
{
    Q_DISABLE_COPY(VolumeToolWidgetPrivate)
public:
    explicit VolumeToolWidgetPrivate(VolumeToolWidget* parent);
    ~VolumeToolWidgetPrivate();
    void initialize();
    void connectAllSlots();
    BmpWidget* m_Background = NULL;
    TextWidget* m_TitleText = NULL;
    VolumeSliderWidget* m_VolumeSliderWidget = NULL;
    short m_Volume = 20;
private:
    VolumeToolWidget* m_Parent = NULL;
};

VolumeToolWidget::VolumeToolWidget(QWidget* parent)
    : QWidget(parent)
    , m_Private(new VolumeToolWidgetPrivate(this))
{
}

VolumeToolWidget::~VolumeToolWidget()
{
}

void VolumeToolWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(213, 180, 853, 360, this);
    g_Widget->geometryFit(0, 0, 853, 360, m_Private->m_Background);
    g_Widget->geometryFit(0, 0, 853, 180, m_Private->m_TitleText);
}

void VolumeToolWidget::changeEvent(QEvent *event)
{
    switch (event->type()) {
    case QEvent::LanguageChange: {
        m_Private->m_TitleText->setText(QString(QObject::tr(SourceString::Volume.toLocal8Bit().constData())) + QString(" ") + QString::number(m_Private->m_Volume));
        break;
    }
    default: {
        break;
    }
    }
    QWidget::changeEvent(event);
}

void VolumeToolWidget::onVolumeChange(const int volume)
{
    qDebug() << "VolumeToolWidget::onVolumeChange" << volume;
    m_Private->m_Volume = volume;
    m_Private->m_TitleText->setText(QString(QObject::tr(SourceString::Volume.toLocal8Bit().constData())) + QString(" ") + QString::number(m_Private->m_Volume));
    m_Private->m_VolumeSliderWidget->setTickMarksMillesimal(1000 * volume / (40 - 0));
}

void VolumeToolWidget::onMinusBtnRelease()
{
    qDebug() << "VolumeToolWidget::onMinusBtnRelease";
    g_Audio->requestDecreaseVolume();
}

void VolumeToolWidget::onPlusBtnRelease()
{
    qDebug() << "VolumeToolWidget::onPlusBtnRelease";
    g_Audio->requestIncreaseVolume();
}

void VolumeToolWidget::onTickMarksMillesimalEnd(const int millesimal)
{
    int volume(((40 - 0) * millesimal / 1000));
    qDebug() << "VolumeToolWidget::onTickMarksMillesimalEnd" << volume;
    g_Audio->requestSetVolume(volume);
}

VolumeToolWidgetPrivate::VolumeToolWidgetPrivate(VolumeToolWidget *parent)
    : m_Parent(parent)
{
    initialize();
    connectAllSlots();
}

VolumeToolWidgetPrivate::~VolumeToolWidgetPrivate()
{
}

void VolumeToolWidgetPrivate::initialize()
{
    m_Background = new BmpWidget(m_Parent);
    m_Background->setBackgroundBmpPath(QString(":/Images/Resources/Images/MessageBackground"));
    m_Background->setVisible(true);
    m_TitleText = new TextWidget(m_Parent);
    m_TitleText->setAlignmentFlag(Qt::AlignCenter);
    m_TitleText->setLanguageType(TextWidget::T_NoTranslate);
    int fontPointSize(40 * g_Widget->widthScalabilityFactor());
    m_TitleText->setFontPointSize(fontPointSize);
//    m_Volume = AudioConfig::getVolume(true);
    m_TitleText->setVisible(true);
    m_TitleText->setText(QString(QObject::tr(SourceString::Volume.toLocal8Bit().constData())) + QString(" ") + QString::number(m_Volume));
    m_VolumeSliderWidget = new VolumeSliderWidget(m_Parent);
    m_VolumeSliderWidget->setTickMarksMillesimal(1000 * m_Volume / (40 - 0));
}

void VolumeToolWidgetPrivate::connectAllSlots()
{
    connectSignalAndSlotByNamesake(g_Audio, m_Parent, SLOT(onVolumeChange(const int)));
    Qt::ConnectionType type = static_cast<Qt::ConnectionType>(Qt::AutoConnection | Qt::UniqueConnection);
    QObject::connect(m_VolumeSliderWidget, SIGNAL(minusBtnRelease()),
                     m_Parent,             SLOT(onMinusBtnRelease()),
                     type);
    QObject::connect(m_VolumeSliderWidget, SIGNAL(plusBtnRelease()),
                     m_Parent,             SLOT(onPlusBtnRelease()),
                     type);
    QObject::connect(m_VolumeSliderWidget, SIGNAL(tickMarksMillesimalEnd(const int)),
                     m_Parent,             SLOT(onTickMarksMillesimalEnd(const int)),
                     type);
}
