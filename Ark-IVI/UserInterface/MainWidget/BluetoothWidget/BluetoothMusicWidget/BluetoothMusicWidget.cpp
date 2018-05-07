#include "BluetoothMusicWidget.h"
#include "AutoConnect.h"
#include "BmpWidget.h"
#include "BmpButton.h"
#include "BusinessLogic/Audio.h"
#include "TextWidget.h"
#include <QDebug>

class BluetoothMusicWidgetPrivate
{
public:
    explicit BluetoothMusicWidgetPrivate(BluetoothMusicWidget* parent);
    ~BluetoothMusicWidgetPrivate();
    void initializeParent();
    void initializeBasicWidget();
    void connectAllSlots();
private:
    Q_DECLARE_PUBLIC(BluetoothMusicWidget)
    BluetoothMusicWidget* const q_ptr;
    BmpWidget* m_BluetoothMusicIcon;
    TextWidget* m_Title;
    BmpButton* m_PreviousBtn;
    BmpButton* m_ToggleBtn;
    BmpButton* m_NextBtn;
};

BluetoothMusicWidget::BluetoothMusicWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new BluetoothMusicWidgetPrivate(this))
{
}

BluetoothMusicWidget::~BluetoothMusicWidget()
{
    if (NULL != d_ptr) {
        delete d_ptr;
    }
}

void BluetoothMusicWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(0, 0, 800, 480, this);
}

void BluetoothMusicWidget::onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status)
{
    switch (destinationType) {
    case Widget::T_BluetoothDial:
    case Widget::T_BluetoothContact:
    case Widget::T_BluetoothRecord:
    case Widget::T_BluetoothSetting: {
        if (WidgetStatus::Show == status) {
            setVisible(false);
        }
        break;
    }
    case Widget::T_BluetoothMusic: {
        if (WidgetStatus::RequestShow == status) {
            g_Widget->setWidgetType(Widget::T_BluetoothMusic, requestType, WidgetStatus::Show);
        } else if (WidgetStatus::Show == status) {
            lower();
            setVisible(true);
            g_Bluetooth->initializeMusic();
        }
        break;
    }
    default: {
        break;
    }
    }
}

void BluetoothMusicWidget::onMusicStatusChange(const QString& musicName, const BluetoothPlayerStatus status)
{
    Q_D(BluetoothMusicWidget);
    d->m_Title->setText(musicName);
    qDebug() << __PRETTY_FUNCTION__ << musicName;
    switch (status) {
    case BPS_Pause: {
        d->m_ToggleBtn->setNormalBmpPath(QString(":/Images/BluetoothMusicPlayNormal.png"));
        d->m_ToggleBtn->setPressBmpPath(QString(":/Images/BluetoothMusicPlayPress.png"));
        break;
    }
    case BPS_Play: {
        d->m_ToggleBtn->setNormalBmpPath(QString(":/Images/BluetoothMusicPauseNormal.png"));
        d->m_ToggleBtn->setPressBmpPath(QString(":/Images/BluetoothMusicPausePress.png"));
        break;
    }
    default: {
        break;
    }
    }
}

void BluetoothMusicWidget::onConnectStatusChange(const BluetoothConnectStatus status)
{
    if ((BCS_Idle == status)
            || (BCS_Connecting == status)) {
        Q_D(BluetoothMusicWidget);
        d->m_ToggleBtn->setNormalBmpPath(QString(":/Images/BluetoothMusicPlayNormal.png"));
        d->m_ToggleBtn->setPressBmpPath(QString(":/Images/BluetoothMusicPlayPress.png"));
        setVisible(false);
    }
}

void BluetoothMusicWidget::onToolButtonRelease()
{
    Q_D(BluetoothMusicWidget);
    BmpButton* ptr = static_cast<BmpButton*>(sender());
    if (ptr == d->m_PreviousBtn) {
        g_Bluetooth->musicPrevious();
    } else if (ptr == d->m_ToggleBtn) {
        g_Bluetooth->musicToggle();
    } else if (ptr == d->m_NextBtn) {
        g_Bluetooth->musicNext();
    }
}

BluetoothMusicWidgetPrivate::BluetoothMusicWidgetPrivate(BluetoothMusicWidget *parent)
    : q_ptr(parent)
{
    m_BluetoothMusicIcon = NULL;
    m_Title = NULL;
    m_PreviousBtn = NULL;
    m_ToggleBtn = NULL;
    m_NextBtn = NULL;
    initializeBasicWidget();
    initializeParent();
    connectAllSlots();
}

BluetoothMusicWidgetPrivate::~BluetoothMusicWidgetPrivate()
{
}

void BluetoothMusicWidgetPrivate::initializeParent()
{
    Q_Q(BluetoothMusicWidget);
    q->setVisible(false);
}

void BluetoothMusicWidgetPrivate::initializeBasicWidget()
{
    Q_Q(BluetoothMusicWidget);
    m_BluetoothMusicIcon = new BmpWidget(q);
    m_BluetoothMusicIcon->setBackgroundBmpPath(QString(":/Images/BluetoothMusicIcon.png"));
    g_Widget->geometryFit(325, 101, 150, 180, m_BluetoothMusicIcon);
    m_BluetoothMusicIcon->setVisible(true);

    m_Title = new TextWidget(q);
    m_Title->setLanguageType(TextWidget::T_NoTranslate);
    g_Widget->geometryFit(0, 247, 800, 62, m_Title);
    m_Title->setVisible(true);

    m_PreviousBtn = new BmpButton(q);
    m_PreviousBtn->setNormalBmpPath(QString(":/Images/BluetoothMusicPreviousNormal.png"));
    m_PreviousBtn->setPressBmpPath(QString(":/Images/BluetoothMusicPreviousPress.png"));
    g_Widget->geometryFit(163, 309, 124, 62, m_PreviousBtn);
    QObject::connect(m_PreviousBtn, ARKSENDER(released()),
                     q,             ARKRECEIVER(onToolButtonRelease()));
    m_PreviousBtn->setVisible(true);

    m_ToggleBtn = new BmpButton(q);
    m_ToggleBtn->setNormalBmpPath(QString(":/Images/BluetoothMusicPlayNormal.png"));
    m_ToggleBtn->setPressBmpPath(QString(":/Images/BluetoothMusicPlayPress.png"));
    g_Widget->geometryFit(340, 309, 124, 62, m_ToggleBtn);
    QObject::connect(m_ToggleBtn, ARKSENDER(released()),
                     q,           ARKRECEIVER(onToolButtonRelease()));
    m_ToggleBtn->setVisible(true);

    m_NextBtn = new BmpButton(q);
    m_NextBtn->setNormalBmpPath(QString(":/Images/BluetoothMusicNextNormal.png"));
    m_NextBtn->setPressBmpPath(QString(":/Images/BluetoothMusicNextPress.png"));
    g_Widget->geometryFit(515, 309, 124, 62, m_NextBtn);
    QObject::connect(m_NextBtn, ARKSENDER(released()),
                     q,         ARKRECEIVER(onToolButtonRelease()));
    m_NextBtn->setVisible(true);
}

void BluetoothMusicWidgetPrivate::connectAllSlots()
{
    Q_Q(BluetoothMusicWidget);
    connectSignalAndSlotByNamesake(g_Widget, q, ARKRECEIVER(onWidgetTypeChange(const Widget::Type, const Widget::Type, const QString &)));
    connectSignalAndSlotByNamesake(g_Bluetooth, q, ARKRECEIVER(onMusicStatusChange(const QString&, const BluetoothPlayerStatus)));
    connectSignalAndSlotByNamesake(g_Bluetooth, q, ARKRECEIVER(onConnectStatusChange(const BluetoothConnectStatus)));
}

