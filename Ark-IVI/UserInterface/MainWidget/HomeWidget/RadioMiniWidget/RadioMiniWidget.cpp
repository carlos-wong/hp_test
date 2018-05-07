#include "RadioMiniWidget.h"
#include "BusinessLogic/Widget.h"
#include "BmpWidget.h"
#include "TextWidget.h"
#include "BmpButton.h"
#include "AutoConnect.h"
#include <QPainter>
#include <QDebug>
#include "BmpWidget.h"

class RadioMiniWidgetPrivate
{
public:
    explicit RadioMiniWidgetPrivate(RadioMiniWidget* parent);
    ~RadioMiniWidgetPrivate();
    void initializeParent();
    void initializeBasic();
    void connectAllSlots();
private:
    Q_DECLARE_PUBLIC(RadioMiniWidget)
    RadioMiniWidget* const q_ptr;
    BmpButton* m_Previous;
    TextWidget* m_Frequency;
    BmpButton* m_Next;
    BmpWidget* m_Logo;
    BmpWidget* m_Band;
};

RadioMiniWidget::RadioMiniWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new RadioMiniWidgetPrivate(this))
{
}

RadioMiniWidget::~RadioMiniWidget()
{
    if (NULL != d_ptr) {
        delete d_ptr;
    }
}

void RadioMiniWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(40, 80, 380, 150, this);
}

void RadioMiniWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.drawPixmap(this->rect(), QPixmap(":/Images/hw_01_big_box_home.png"));
}

void RadioMiniWidget::onRadioInfoChange(const RadioBandType type, const unsigned short frequency, const unsigned short index, const unsigned short min, const unsigned short max, const unsigned short step)
{
    QString frequencyText;
    switch (type) {
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
    Q_D(RadioMiniWidget);
    d->m_Frequency->setText(frequencyText);
}

void RadioMiniWidget::onBmpButtonRelease()
{
    BmpButton* ptr = static_cast<BmpButton*>(sender());
    Q_D(RadioMiniWidget);
    int index = g_Radio->getCurrendBandIndex();
    qDebug() << __PRETTY_FUNCTION__ << "in" << index;
    if (0 == index) {
        index = 1;
    } else {
        if (ptr == d->m_Previous) {
            index--;
            if ((index < 1) || (index > 6)) {
                index = 6;
            }
        } else if (ptr == d->m_Next) {
            index++;
            if (index > 6) {
                index = 1;
            }
        }
    }
    qDebug() << __PRETTY_FUNCTION__ << "out" << index;
    g_Radio->setCurrentBandIndex(index);
}


RadioMiniWidgetPrivate::RadioMiniWidgetPrivate(RadioMiniWidget *parent)
    : q_ptr(parent)
{
    m_Previous = NULL;
    m_Frequency = NULL;
    m_Next = NULL;
    m_Logo = NULL;
    m_Band = NULL;
    initializeBasic();
    initializeParent();
    connectAllSlots();
}

RadioMiniWidgetPrivate::~RadioMiniWidgetPrivate()
{
}

void RadioMiniWidgetPrivate::initializeParent()
{
    Q_Q(RadioMiniWidget);
    q->setVisible(false);
}

void RadioMiniWidgetPrivate::initializeBasic()
{
    Q_Q(RadioMiniWidget);

    Qt::ConnectionType type = static_cast<Qt::ConnectionType>(Qt::AutoConnection | Qt::UniqueConnection);
    m_Previous = new BmpButton(q);
    m_Previous->setNormalBmpPath(QString(":/Images/hw_01_previous_mili_normal.png"));
    m_Previous->setPressBmpPath(QString(":/Images/hw_01_previous_mili_pressed.png"));
    g_Widget->geometryFit(50, 105, 40, 40, m_Previous);
    QObject::connect(m_Previous, ARKSENDER(released()),
                     q,          ARKRECEIVER(onBmpButtonRelease()),
                     type);

    m_Logo = new BmpWidget(q);
    g_Widget->geometryFit(30, 20, 75,75, m_Logo);
    m_Logo->setBackgroundBmpPath(":/Images/hw_01_radio_mili_logol.png");

    m_Band = new BmpWidget(q);
    g_Widget->geometryFit(130, 20, 220,75, m_Band);
    m_Band->setBackgroundBmpPath(":/Images/hw_01_radio_band_mili.png");

    m_Frequency = new TextWidget(q);
    m_Frequency->setLanguageType(TextWidget::T_NoTranslate);
    m_Frequency->setFontPointSize(12 * g_Widget->widthScalabilityFactor());
    m_Frequency->setAlignmentFlag(Qt::AlignCenter);
    g_Widget->geometryFit(140, 105, 100, 40, m_Frequency);
    m_Frequency->setVisible(true);

    m_Next = new BmpButton(q);
    m_Next->setNormalBmpPath(QString(":/Images/hw_01_next_mili_normal.png"));
    m_Next->setPressBmpPath(QString(":/Images/hw_01_next_mili_pressed.png"));
    g_Widget->geometryFit(290, 105, 40, 40, m_Next);
    QObject::connect(m_Next, ARKSENDER(released()),
                     q,      ARKRECEIVER(onBmpButtonRelease()),
                     type);
    QString frequencyText;
    const unsigned short frequency = g_Radio->getCurrendBandFrequency();
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
}

void RadioMiniWidgetPrivate::connectAllSlots()
{
    Q_Q(RadioMiniWidget);
    connectSignalAndSlotByNamesake(g_Radio, q, ARKRECEIVER(onRadioInfoChange(const RadioBandType , const unsigned short , const unsigned short , const unsigned short , const unsigned short , const unsigned short)));
}
