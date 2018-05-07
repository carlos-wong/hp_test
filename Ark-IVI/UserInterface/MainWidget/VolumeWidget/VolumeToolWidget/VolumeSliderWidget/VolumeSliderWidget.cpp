#include "VolumeSliderWidget.h"
#include "BmpWidget.h"
#include "BusinessLogic/Widget.h"
#include "BmpButton.h"
#include "TextWidget.h"
#include "Slider.h"
#include "AutoConnect.h"
#include "UserInterfaceUtility.h"

class VolumeSliderWidgetPrivate
{
    Q_DISABLE_COPY(VolumeSliderWidgetPrivate)
public:
    explicit VolumeSliderWidgetPrivate(VolumeSliderWidget* parent);
    ~VolumeSliderWidgetPrivate();
    void initialize();
    void connectAllSlots();
    BmpButton* m_MinusBtn = NULL;
//    Slider* m_Slider = NULL;
    BmpButton* m_PlusBtn = NULL;
private:
    VolumeSliderWidget* m_Parent = NULL;
};

VolumeSliderWidget::VolumeSliderWidget(QWidget* parent)
    : QWidget(parent)
    , m_Private(new VolumeSliderWidgetPrivate(this))
{
    setVisible(true);
}

VolumeSliderWidget::~VolumeSliderWidget()
{
}

void VolumeSliderWidget::setTickMarksMillesimal(const int millesimal)
{
//    m_Private->m_Slider->setTickMarksMillesimal(millesimal);
}

void VolumeSliderWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(0, 180, 853, 180, this);
    g_Widget->geometryFit(52 - 20, (180 - 92) * 0.5, 92, 92, m_Private->m_MinusBtn);
//    g_Widget->geometryFit(117, (180 - 92) * 0.5, 619, 92, m_Private->m_Slider);
    g_Widget->geometryFit(853 - 52 - 92 + 20, (180 - 92) * 0.5, 92, 92, m_Private->m_PlusBtn);
}

VolumeSliderWidgetPrivate::VolumeSliderWidgetPrivate(VolumeSliderWidget *parent)
    : m_Parent(parent)
{
    initialize();
    connectAllSlots();
}

VolumeSliderWidgetPrivate::~VolumeSliderWidgetPrivate()
{
}

void VolumeSliderWidgetPrivate::initialize()
{
    m_MinusBtn = new BmpButton(m_Parent);
    m_MinusBtn->setNormalBmpPath(QString(":/Images/Resources/Images/EffectSoundSliderMinusNormal"));
    m_MinusBtn->setPressBmpPath(QString(":/Images/Resources/Images/EffectSoundSliderMinusNormal"));
    m_MinusBtn->setVisible(true);
    m_PlusBtn = new BmpButton(m_Parent);
    m_PlusBtn->setNormalBmpPath(QString(":/Images/Resources/Images/EffectSoundSliderPlusNormal"));
    m_PlusBtn->setPressBmpPath(QString(":/Images/Resources/Images/EffectSoundSliderPlusNormal"));
    m_PlusBtn->setVisible(true);
//    m_Slider = new Slider(m_Parent);
//    m_Slider->setTickMarksSize(QSize(40 * g_Widget->widthScalabilityFactor(), 40 * g_Widget->heightScalabilityFactor()));
//    m_Slider->setBackgroundBmpPath(QString(":/Images/Resources/Images/EffectSoundSliderBackground"));
//    m_Slider->setTickMarkBmpPath(QString(":/Images/Resources/Images/EffectSoundSliderTickMarksBackground"));
//    m_Slider->setVisible(true);
}

void VolumeSliderWidgetPrivate::connectAllSlots()
{
    Qt::ConnectionType type = static_cast<Qt::ConnectionType>(Qt::AutoConnection | Qt::UniqueConnection);
    QObject::connect(m_MinusBtn, SIGNAL(released()),
                     m_Parent,   SIGNAL(minusBtnRelease()),
                     type);
//    QObject::connect(m_Slider, SIGNAL(tickMarksMillesimalEnd(const int)),
//                     m_Parent, SIGNAL(tickMarksMillesimalEnd(const int)),
//                     type);
    QObject::connect(m_PlusBtn, SIGNAL(released()),
                     m_Parent,  SIGNAL(plusBtnRelease()),
                     type); 
}
