#include "RadioUtilityWidget.h"
#include "AutoConnect.h"
#include "BmpButton.h"
#include "TextWidget.h"
#include "BmpWidget.h"
#include "BusinessLogic/Widget.h"
#include <QDebug>

namespace SourceString {
static const QString Stereo(QObject::tr("Stereo"));
}

class RadioUtilityWidgetPrivate
{
public:
    explicit RadioUtilityWidgetPrivate(RadioUtilityWidget* parent);
    ~RadioUtilityWidgetPrivate();
    void initialize();
    void initializeParent();
    void connectAllSlots();
    void initializeUsedStereoTip();
    void initializeListenStereoTip();
    void initializeLocTip();
    void initializeBandTip();
    void initializeFrequencyTip();
    void initializeUnitTip();
    void setBand(const RadioBandType type);
    void setFrequency(const unsigned short frequency);
private:
    Q_DECLARE_PUBLIC(RadioUtilityWidget)
    RadioUtilityWidget* const q_ptr;
    TextWidget* m_UsedStereoTip;
    TextWidget* m_ListenStereoTip;
    TextWidget* m_LocTip;
    BmpButton* m_MultiplexingSubBtn;
    TextWidget* m_Frequency;
    TextWidget* m_Band;
    TextWidget* m_Unit;
    BmpButton* m_MultiplexingAddBtn;
};

RadioUtilityWidget::RadioUtilityWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new RadioUtilityWidgetPrivate(this))
{
}

RadioUtilityWidget::~RadioUtilityWidget()
{
    if (NULL != d_ptr) {
        delete d_ptr;
    }
}

void RadioUtilityWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(0, 122, 800, 200, this);
}

void RadioUtilityWidget::onSliderMoved(int value)
{
    Q_D(RadioUtilityWidget);
    int step = g_Radio->getCurrentBandStep();
    d->setFrequency((value / step) * step);
}

void RadioUtilityWidget::onUsedStereoChange(const RadioStereoType type)
{
    Q_D(RadioUtilityWidget);
    d->initializeUsedStereoTip();
    d->m_UsedStereoTip->setVisible(RST_Stereo == type);
}

void RadioUtilityWidget::onListenStereoChange(const RadioStereoType type)
{
    Q_D(RadioUtilityWidget);
    d->initializeListenStereoTip();
    d->m_ListenStereoTip->setVisible(RST_Stereo == type);
}

void RadioUtilityWidget::onLocChange(const RadioLocType type)
{
    Q_D(RadioUtilityWidget);
    d->initializeLocTip();
    d->m_LocTip->setVisible(RLT_LocalSensitivityOn == type);
}

void RadioUtilityWidget::onRadioInfoChange(const RadioBandType type, const unsigned short frequency, const unsigned short index, const unsigned short min, const unsigned short max, const unsigned short step)
{
    Q_D(RadioUtilityWidget);
    d->initializeBandTip();
    d->initializeUnitTip();
    d->setBand(type);
    d->initializeFrequencyTip();
    d->setFrequency(frequency);
}

void RadioUtilityWidget::onToolButtonRelease()
{
    Q_D(RadioUtilityWidget);
    BmpButton* ptr = static_cast<BmpButton*>(sender());
    if (ptr == d->m_MultiplexingSubBtn) {
        g_Radio->setCurrentBandTurnSub();
    } else if (ptr == d->m_MultiplexingAddBtn) {
        g_Radio->setCurrentBandTurnAdd();
    }
}

void RadioUtilityWidget::onToolButtonLongPressed()
{
    Q_D(RadioUtilityWidget);
    BmpButton* ptr = static_cast<BmpButton*>(sender());
    if (ptr == d->m_MultiplexingSubBtn) {
        g_Radio->setSearchPreviousStation();
    } else if (ptr == d->m_MultiplexingAddBtn) {
        g_Radio->setSearchNextStation();
    }
}

RadioUtilityWidgetPrivate::RadioUtilityWidgetPrivate(RadioUtilityWidget *parent)
    : q_ptr(parent)
{
    m_UsedStereoTip = NULL;
    m_ListenStereoTip = NULL;
    m_LocTip = NULL;
    m_MultiplexingSubBtn = NULL;
    m_Frequency = NULL;
    m_Band = NULL;
    m_Unit = NULL;
    m_MultiplexingAddBtn = NULL;
    initialize();
    initializeParent();
    connectAllSlots();
}

RadioUtilityWidgetPrivate::~RadioUtilityWidgetPrivate()
{
}

void RadioUtilityWidgetPrivate::initialize()
{
    Q_Q(RadioUtilityWidget);
    if (RST_Stereo == g_Radio->getUsedStereoType()) {
        initializeUsedStereoTip();
        m_UsedStereoTip->setVisible(true);
    }
    if (RLT_LocalSensitivityOn == g_Radio->getLocType()) {
        initializeLocTip();
        m_LocTip->setVisible(true);
    }
    initializeFrequencyTip();
    setFrequency(g_Radio->getCurrendBandFrequency());
    initializeUnitTip();
    initializeBandTip();
    setBand(g_Radio->getCurrentBand());
    m_MultiplexingSubBtn = new BmpButton(q);
    m_MultiplexingSubBtn->setNormalBmpPath(QString(":/Images/hw_01_radio_back_normal.png"));
    m_MultiplexingSubBtn->setPressBmpPath(QString(":/Images/hw_01_radio_back_pressed.png"));
    m_MultiplexingSubBtn->setCheckBmpPath(QString(":/Images/hw_01_radio_back_pressed.png"));
    g_Widget->geometryFit(50, 60, 84, 84, m_MultiplexingSubBtn);
    m_MultiplexingSubBtn->enableLongPress(true);
    m_MultiplexingSubBtn->setVisible(true);
    QObject::connect(m_MultiplexingSubBtn, ARKSENDER(released()),
                     q,                    ARKRECEIVER(onToolButtonRelease()));
    QObject::connect(m_MultiplexingSubBtn, ARKSENDER(longPressed()),
                     q,                    ARKRECEIVER(onToolButtonLongPressed()));
    m_MultiplexingAddBtn = new BmpButton(q);
    m_MultiplexingAddBtn->setNormalBmpPath(QString(":/Images/hw_01_radio_forward_normal.png"));
    m_MultiplexingAddBtn->setPressBmpPath(QString(":/Images/hw_01_radio_forward_pressed.png"));
    m_MultiplexingAddBtn->setCheckBmpPath(QString(":/Images/hw_01_radio_forward_pressed.png"));
    g_Widget->geometryFit(666, 60, 84, 84, m_MultiplexingAddBtn);
    m_MultiplexingAddBtn->enableLongPress(true);
    m_MultiplexingAddBtn->setVisible(true);
    QObject::connect(m_MultiplexingAddBtn, ARKSENDER(released()),
                     q,                    ARKRECEIVER(onToolButtonRelease()));
    QObject::connect(m_MultiplexingAddBtn, ARKSENDER(longPressed()),
                     q,                    ARKRECEIVER(onToolButtonLongPressed()));
}

void RadioUtilityWidgetPrivate::initializeParent()
{
    Q_Q(RadioUtilityWidget);
    q->setVisible(true);
}

void RadioUtilityWidgetPrivate::connectAllSlots()
{
    Q_Q(RadioUtilityWidget);
    connectSignalAndSlotByNamesake(g_Radio, q, ARKRECEIVER(onLocChange(const RadioLocType)));
    connectSignalAndSlotByNamesake(g_Radio, q, ARKRECEIVER(onUsedStereoChange(const RadioStereoType)));
    connectSignalAndSlotByNamesake(g_Radio, q, ARKRECEIVER(onListenStereoChange(const RadioStereoType)));
    connectSignalAndSlotByNamesake(g_Radio, q, ARKRECEIVER(onRadioInfoChange(const RadioBandType , const unsigned short , const unsigned short , const unsigned short , const unsigned short , const unsigned short)));
}

void RadioUtilityWidgetPrivate::initializeUsedStereoTip()
{
    if (NULL == m_UsedStereoTip) {
        Q_Q(RadioUtilityWidget);
        m_UsedStereoTip = new TextWidget(q);
        m_UsedStereoTip->setVisible(false);
        m_UsedStereoTip->setText(SourceString::Stereo);
        m_UsedStereoTip->setFontPointSize(9 * g_Widget->widthScalabilityFactor());
        g_Widget->geometryFit(325, 0, 50, 50, m_UsedStereoTip);
    }
}

void RadioUtilityWidgetPrivate::initializeListenStereoTip()
{
    if (NULL == m_ListenStereoTip) {
        Q_Q(RadioUtilityWidget);
        m_ListenStereoTip = new TextWidget(q);
        m_ListenStereoTip->setVisible(false);
        m_ListenStereoTip->setText(QString("ST"));
        m_ListenStereoTip->setFontPointSize(9 * g_Widget->widthScalabilityFactor());
        g_Widget->geometryFit(375, 0, 50, 50, m_ListenStereoTip);
    }
}

void RadioUtilityWidgetPrivate::initializeLocTip()
{
    if (NULL == m_LocTip) {
        Q_Q(RadioUtilityWidget);
        m_LocTip = new TextWidget(q);
        m_LocTip->setVisible(false);
        m_LocTip->setText(QString("LOC"));
        m_LocTip->setFontPointSize(9 * g_Widget->widthScalabilityFactor());
        g_Widget->geometryFit(425, 0, 50, 50, m_LocTip);
    }
}

void RadioUtilityWidgetPrivate::initializeBandTip()
{
    if (NULL == m_Band) {
        Q_Q(RadioUtilityWidget);
        m_Band = new TextWidget(q);
        m_Band->setVisible(false);
        m_Band->setLanguageType(TextWidget::T_NoTranslate);
        m_Band->setFontPointSize(11 * g_Widget->widthScalabilityFactor());
        g_Widget->geometryFit(536, 77, 58, 26, m_Band);
    }
}

void RadioUtilityWidgetPrivate::initializeFrequencyTip()
{
    if (NULL == m_Frequency) {
        Q_Q(RadioUtilityWidget);
        m_Frequency = new TextWidget(q);
        m_Frequency->setVisible(false);
        m_Frequency->setLanguageType(TextWidget::T_NoTranslate);
        m_Frequency->setFontPointSize(54 * g_Widget->widthScalabilityFactor());
        m_Frequency->setAlignmentFlag(Qt::AlignRight | Qt::AlignVCenter);
        g_Widget->geometryFit(192, 60, 322, 84, m_Frequency);
    }
}

void RadioUtilityWidgetPrivate::initializeUnitTip()
{
    if (NULL == m_Unit) {
        Q_Q(RadioUtilityWidget);
        m_Unit = new TextWidget(q);
        m_Unit->setVisible(false);
        m_Unit->setLanguageType(TextWidget::T_NoTranslate);
        m_Unit->setFontPointSize(15 * g_Widget->widthScalabilityFactor());
        g_Widget->geometryFit(536, 112, 58, 26, m_Unit);
    }
}

void RadioUtilityWidgetPrivate::setBand(const RadioBandType type)
{
    switch (type) {
    case RBT_FM1: {
        if (NULL != m_Band) {
            m_Band->setText(QString("FM1"));
            m_Band->setVisible(true);
        }
        if (NULL != m_Unit) {
            m_Unit->setText(QString("MHz"));
            m_Unit->setVisible(true);
        }
        break;
    }
    case RBT_FM2: {
        if (NULL != m_Band) {
            m_Band->setText(QString("FM2"));
            m_Band->setVisible(true);
        }
        if (NULL != m_Unit) {
            m_Unit->setText(QString("MHz"));
            m_Unit->setVisible(true);
        }
        break;
    }
    case RBT_FM3: {
        if (NULL != m_Band) {
            m_Band->setText(QString("FM3"));
            m_Band->setVisible(true);
        }
        if (NULL != m_Unit) {
            m_Unit->setText(QString("MHz"));
            m_Unit->setVisible(true);
        }
        break;
    }
    case RBT_AM1: {
        if (NULL != m_Band) {
            m_Band->setText(QString("AM1"));
            m_Band->setVisible(true);
        }
        if (NULL != m_Unit) {
            m_Unit->setText(QString("KHz"));
            m_Unit->setVisible(true);
        }
        break;
    }
    case RBT_AM2: {
        if (NULL != m_Band) {
            m_Band->setText(QString("AM2"));
            m_Band->setVisible(true);
        }
        if (NULL != m_Unit) {
            m_Unit->setText(QString("KHz"));
            m_Unit->setVisible(true);
        }
        break;
    }
    default: {
        break;
    }
    }
}

void RadioUtilityWidgetPrivate::setFrequency(const unsigned short frequency)
{
    if (NULL != m_Frequency) {
        QString frequencyText;
        switch (g_Radio->getCurrentBand()) {
        case RBT_FM1:
        case RBT_FM2:
        case RBT_FM3: {
            QString decimal("%1");
            frequencyText = QString::number(frequency / 100) + QString(".") + decimal.arg(frequency % 100, 2, 10, QChar('0'));
            break;
        }
        case RBT_AM1:
        case RBT_AM2: {
            frequencyText = QString::number(frequency);
            break;
        }
        default: {
            return;
            break;
        }
        }
        m_Frequency->setText(frequencyText);
        m_Frequency->setVisible(true);
    }
}
