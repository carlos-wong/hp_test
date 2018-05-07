#include "SettingGeneralSlider.h"
#include "AutoConnect.h"
#include "TextWidget.h"
#include "BmpWidget.h"
#include "Slider.h"
#include "BusinessLogic/Widget.h"
#include <QPainter>

class SettingGeneralSliderPrivate
{
public:
    explicit SettingGeneralSliderPrivate(SettingGeneralSlider* parent);
    ~SettingGeneralSliderPrivate();
    void initializeParent();
    void initializeBasicWidget();
private:
    Q_DECLARE_PUBLIC(SettingGeneralSlider)
    SettingGeneralSlider* const q_ptr;
    BmpWidget* m_Background;
    TextWidget* m_Text;
    TextWidget* m_MinimumText;
    TextWidget* m_MaximumText;
    Slider* m_Slider;
};

SettingGeneralSlider::SettingGeneralSlider(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new SettingGeneralSliderPrivate(this))
{
}

SettingGeneralSlider::~SettingGeneralSlider()
{
    if (NULL != d_ptr) {
        delete d_ptr;
    }
}

void SettingGeneralSlider::setText(const QString &text)
{
    Q_D(SettingGeneralSlider);
    d->m_Text->setText(text);
}

void SettingGeneralSlider::setRange(const int minimum, const int maximum)
{
    Q_D(SettingGeneralSlider);
    d->m_Slider->setRange(minimum, maximum);
    d->m_MinimumText->setText(QString::number(minimum));
}

void SettingGeneralSlider::setValue(const int value)
{
    Q_D(SettingGeneralSlider);
    if (!d->m_Slider->isSliderDown()) {
        d->m_Slider->setValue(value);
    }
    d->m_MaximumText->setText(QString::number(value));
}

int SettingGeneralSlider::value()
{
    Q_D(SettingGeneralSlider);
    return d->m_Slider->value();
}

bool SettingGeneralSlider::isSliderDown()
{
    Q_D(SettingGeneralSlider);
    return d->m_Slider->isSliderDown();
}

void SettingGeneralSlider::resizeEvent(QResizeEvent *event)
{
    Q_D(SettingGeneralSlider);
    d->m_Background->move(0, height() - 2);
    d->m_Background->resize(width(), 2);
    d->m_Text->resize(133 * g_Widget->widthScalabilityFactor(), height() * g_Widget->heightScalabilityFactor());
    d->m_MinimumText->resize(67 * g_Widget->widthScalabilityFactor(), height() * g_Widget->heightScalabilityFactor());
    d->m_MaximumText->resize(67 * g_Widget->widthScalabilityFactor(), height() * g_Widget->heightScalabilityFactor());
    d->m_Slider->resize(334 * g_Widget->widthScalabilityFactor(), height() * g_Widget->heightScalabilityFactor());
}

SettingGeneralSliderPrivate::SettingGeneralSliderPrivate(SettingGeneralSlider *parent)
    : q_ptr(parent)
{
    m_Background = NULL;
    m_Text = NULL;
    m_MinimumText = NULL;
    m_MaximumText = NULL;
    m_Slider = NULL;
    initializeBasicWidget();
    initializeParent();
}

SettingGeneralSliderPrivate::~SettingGeneralSliderPrivate()
{
}

void SettingGeneralSliderPrivate::initializeParent()
{
    Q_Q(SettingGeneralSlider);
    q->setVisible(false);
}

void SettingGeneralSliderPrivate::initializeBasicWidget()
{
    Q_Q(SettingGeneralSlider);
    m_Background = new BmpWidget(q);
    m_Background->setBackgroundBmpPath(QString(":/Images/MultimediaListViewRules.png"));
    m_Background->setVisible(true);

    m_Text = new TextWidget(q);
    m_Text->setFontPointSize(11 * g_Widget->widthScalabilityFactor());
    m_Text->setAlignmentFlag(Qt::AlignLeft | Qt::AlignVCenter);
    m_Text->move(32 * g_Widget->widthScalabilityFactor(), 0);
    m_Text->setVisible(true);

    m_MinimumText = new TextWidget(q);
    m_MinimumText->setFontPointSize(11 * g_Widget->widthScalabilityFactor());
    m_MinimumText->setAlignmentFlag(Qt::AlignCenter);
    m_MinimumText->setLanguageType(TextWidget::T_NoTranslate);
    m_MinimumText->move(100 * g_Widget->widthScalabilityFactor(), 0);
    m_MinimumText->setVisible(true);

    m_MaximumText = new TextWidget(q);
    m_MaximumText->setFontPointSize(11 * g_Widget->widthScalabilityFactor());
    m_MaximumText->setAlignmentFlag(Qt::AlignCenter);
    m_MaximumText->setLanguageType(TextWidget::T_NoTranslate);
    m_MaximumText->move(501 * g_Widget->widthScalabilityFactor(), 0);
    m_MaximumText->setVisible(true);

    m_Slider = new Slider(q);
    m_Slider->setStyleSheet(QString("QSlider::groove:horizontal {"
                                    "height: " + QString::number(int(24 * g_Widget->widthScalabilityFactor())) + "px;"
                                                                                                                 "background: transparent;"
                                                                                                                 "}"
                                                                                                                 "QSlider::handle:horizontal {"
                                                                                                                 "width: " + QString::number(int(24 * g_Widget->widthScalabilityFactor())) + "px;"
                                                                                                                                                                                             "image: url(:/Images/SettingGeneralSeekBaTickMarks.png);"
                                                                                                                                                                                             "}"
                                                                                                                                                                                             "QSlider::sub-page:horizontal {"
                                                                                                                                                                                             "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #003e61, stop: 1 #009ffa); "
                                                                                                                                                                                             "margin: " + QString::number(int(7 * g_Widget->widthScalabilityFactor())) + "px 0px " + QString::number(int(7 * g_Widget->widthScalabilityFactor())) + "px 0px;"
                                                                                                                                                                                                                                                                                                                                                    "border-radius: " + QString::number(int(5 * g_Widget->widthScalabilityFactor())) + "px;"
                                                                                                                                                                                                                                                                                                                                                                                                                                       "}"
                                                                                                                                                                                                                                                                                                                                                                                                                                       "QSlider::add-page:horizontal {"
                                                                                                                                                                                                                                                                                                                                                                                                                                       "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #616161, stop: 1 #2b2b2b); "
                                                                                                                                                                                                                                                                                                                                                                                                                                       "margin: " + QString::number(int(7 * g_Widget->widthScalabilityFactor())) + "px 0px " + QString::number(int(7 * g_Widget->widthScalabilityFactor())) + "px 0px;"
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              "border-radius: " + QString::number(int(5 * g_Widget->widthScalabilityFactor())) + "px;"
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 "}"));
    m_Slider->move(167 * g_Widget->widthScalabilityFactor(), 0);
    m_Slider->setVisible(true);
    Qt::ConnectionType type = static_cast<Qt::ConnectionType>(Qt::UniqueConnection | Qt::AutoConnection);
    QObject::connect(m_Slider, ARKSENDER(sliderMoved(const int)),
                     q,        ARKSENDER(sliderMoved(const int)),
                     type);
    QObject::connect(m_Slider, ARKSENDER(sliderReleased(const int)),
                     q,        ARKSENDER(sliderReleased(const int)),
                     type);
}
