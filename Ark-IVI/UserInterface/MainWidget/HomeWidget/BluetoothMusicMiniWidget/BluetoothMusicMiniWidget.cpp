#include "BluetoothMusicMiniWidget.h"
#include "BmpWidget.h"
#include "BmpButton.h"
#include "AutoConnect.h"
#include "TextWidget.h"
#include "BusinessLogic/Audio.h"
#include <QPainter>
#include <QDebug>

class BluetoothMusicMiniWidgetPrivate
{
public:
    BluetoothMusicMiniWidgetPrivate(BluetoothMusicMiniWidget* parent);
    ~BluetoothMusicMiniWidgetPrivate();
    void initializeParent();
    void initializeBasic();
    void connectAllSlots();
private:
    Q_DECLARE_PUBLIC(BluetoothMusicMiniWidget)
    BluetoothMusicMiniWidget* const q_ptr;
    BmpButton* m_Previous;
    BmpButton* m_Toggle;
    BmpButton* m_Next;
    BmpWidget *m_MusicLogo;
    TextWidget *m_MusicName;
};

BluetoothMusicMiniWidget::BluetoothMusicMiniWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new BluetoothMusicMiniWidgetPrivate(this))
{
}

BluetoothMusicMiniWidget::~BluetoothMusicMiniWidget()
{
    if (NULL != d_ptr) {
        delete d_ptr;
    }
}

void BluetoothMusicMiniWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(40, 80, 380, 150, this);
}

void BluetoothMusicMiniWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.drawPixmap(this->rect(), QPixmap(":/Images/hw_01_big_box_home.png"));
}

void BluetoothMusicMiniWidget::onMusicStatusChange(const QString& musicName, const BluetoothPlayerStatus status)
{
    Q_D(BluetoothMusicMiniWidget);
    d->m_MusicName->setText(musicName);
    switch (status) {
    case BPS_Pause: {
        d->m_Toggle->setNormalBmpPath(QString(":/Images/hw_01_play_mili_normal.png"));
        d->m_Toggle->setPressBmpPath(QString(":/Images/hw_01_play_mili_pressed.png"));
        break;
    }
    case BPS_Play: {
        d->m_Toggle->setNormalBmpPath(QString(":/Images/hw_01_pause_mili_normal.png"));
        d->m_Toggle->setPressBmpPath(QString(":/Images/hw_01_pause_mili_pressed.png"));
        break;
    }
    default: {
        break;
    }
    }
}

void BluetoothMusicMiniWidget::onBmpButtonRelease()
{
    qDebug() << __PRETTY_FUNCTION__;
    BmpButton* ptr = static_cast<BmpButton*>(sender());
    Q_D(BluetoothMusicMiniWidget);
    if (ptr == d->m_Previous) {
        g_Bluetooth->musicPrevious();
    } else if (ptr == d->m_Toggle) {
        g_Bluetooth->musicToggle();
    } else if (ptr == d->m_Next) {
        g_Bluetooth->musicNext();
    }
}

BluetoothMusicMiniWidgetPrivate::BluetoothMusicMiniWidgetPrivate(BluetoothMusicMiniWidget *parent)
    : q_ptr(parent)
{
    m_Previous = NULL;
    m_Toggle = NULL;
    m_Next = NULL;
    m_MusicLogo = NULL;
    m_MusicName = NULL;
    initializeBasic();
    initializeParent();
    connectAllSlots();
}

BluetoothMusicMiniWidgetPrivate::~BluetoothMusicMiniWidgetPrivate()
{
}

void BluetoothMusicMiniWidgetPrivate::initializeParent()
{
    Q_Q(BluetoothMusicMiniWidget);
    q->setVisible(false);
}

void BluetoothMusicMiniWidgetPrivate::initializeBasic()
{
    Q_Q(BluetoothMusicMiniWidget);

    Qt::ConnectionType type = static_cast<Qt::ConnectionType>(Qt::AutoConnection | Qt::UniqueConnection);
    m_Previous = new BmpButton(q);
    m_Previous->setNormalBmpPath(QString(":/Images/hw_01_previous_mili_normal.png"));
    m_Previous->setPressBmpPath(QString(":/Images/hw_01_previous_mili_pressed.png"));
    g_Widget->geometryFit(50, 105, 40, 40, m_Previous);
    QObject::connect(m_Previous, ARKSENDER(released()),
                     q,          ARKRECEIVER(onBmpButtonRelease()),
                     type);
    m_Toggle = new BmpButton(q);
    m_Toggle->setNormalBmpPath(QString(":/Images/hw_01_play_mili_normal.png"));
    m_Toggle->setPressBmpPath(QString(":/Images/hw_01_play_mili_pressed.png"));
    g_Widget->geometryFit(170, 105, 40, 40, m_Toggle);
    QObject::connect(m_Toggle, ARKSENDER(released()),
                     q,        ARKRECEIVER(onBmpButtonRelease()),
                     type);
    m_Next = new BmpButton(q);
    m_Next->setNormalBmpPath(QString(":/Images/hw_01_next_mili_normal.png"));
    m_Next->setPressBmpPath(QString(":/Images/hw_01_next_mili_pressed.png"));
    g_Widget->geometryFit(290, 105, 40, 40, m_Next);
    QObject::connect(m_Next, ARKSENDER(released()),
                     q,      ARKRECEIVER(onBmpButtonRelease()),
                     type);

    m_MusicLogo = new BmpWidget(q);
    m_MusicLogo->setBackgroundBmpPath(":/Images/hw_01_bluetooth_mili_logol.png");
    g_Widget->geometryFit(30, 20, 75,75, m_MusicLogo);

    m_MusicName = new TextWidget(q);
    g_Widget->geometryFit(120, 20, 225, 40, m_MusicName);
    m_MusicName->setLanguageType(TextWidget::T_NoTranslate);
    m_MusicName->setAlignmentFlag(Qt::AlignLeft | Qt::AlignVCenter);
    m_MusicName->setFontPointSize(12 * g_Widget->widthScalabilityFactor());
    m_MusicName->setAnimation(true);
    m_MusicName->setText(QString("Unknown"));
}

void BluetoothMusicMiniWidgetPrivate::connectAllSlots()
{
    Q_Q(BluetoothMusicMiniWidget);
    connectSignalAndSlotByNamesake(g_Bluetooth, q, ARKRECEIVER(onMusicStatusChange(const QString&, const BluetoothPlayerStatus)));
}
