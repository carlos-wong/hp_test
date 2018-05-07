#include "VolumeWidget.h"
#include "BusinessLogic/BusinessLogicUtility.h"
#include "BusinessLogic/Audio.h"
#include "BusinessLogic/Widget.h"
#include "AutoConnect.h"
#include "VolumeToolWidget/VolumeToolWidget.h"
#include "BmpWidget.h"
#include "TextWidget.h"
#include <QVariant>
#include <QPainter>
#include <QEvent>
#include <QTimer>
#include <QMouseEvent>
#include <QTime>

class VolumeWidgetPrivate
{
    Q_DISABLE_COPY(VolumeWidgetPrivate)
public:
    explicit VolumeWidgetPrivate(VolumeWidget* parent);
    ~VolumeWidgetPrivate();
    void initializeParent();
    void initializeBackground();
    void initializeVolumeIcon();
    void initializeTextWidget();
    void initializeTimer();
    void initializeLongPressTimer();
    void connectAllSlots();
    QTimer* m_Timer;
    BmpWidget* m_Background;
    BmpWidget* m_VolumeIcon;
    TextWidget* m_TextWidget;
    int m_Volume;
    bool m_Show;
    QTimer* m_LongPressTimer;
private:
    VolumeWidget* m_Parent;
};

VolumeWidget::VolumeWidget(QWidget *parent)
    : QWidget(parent)
    , m_Private(new VolumeWidgetPrivate(this))
{
}

VolumeWidget::~VolumeWidget()
{
}

void VolumeWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.fillRect(rect(), QColor(0, 0, 0, 80));
}

void VolumeWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(0, 0, 800, 480, this);
}

void VolumeWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (isVisible()) {
        m_Private->m_Timer->stop();
        setVisible(false);
    }
}

void VolumeWidget::onArkProtocol(const ArkProtocolWrapper &protocol)
{
    switch (protocol.type) {
    case AT_Key: {
        struct ArkKey* key = arkProtocolWrapperToArkKey(protocol);
        if ((NULL != key)
                && (!protocol.handler)) {
            connectSignalAndSlotByNamesake(g_Audio, this, ARKRECEIVER(onMuteChange(const int)));
            connectSignalAndSlotByNamesake(g_Audio, this, ARKRECEIVER(onVolumeChange(const int)));
            switch (key->type) {
            case AKT_CommonVolumeIncrement: {
                if ((AKS_Release == key->status)
                        || (AKS_Active == key->status)) {
                    m_Private->m_Show = true;
                    g_Audio->requestIncreaseVolume();
                }
                break;
            }
            case AKT_CommonVolumeDecrement: {
                switch (key->status) {
                case AKS_Active: {
                    m_Private->m_Show = true;
                    g_Audio->requestDecreaseVolume();
                    break;
                }
                case AKS_Press: {
                    m_Private->initializeLongPressTimer();
                    m_Private->m_LongPressTimer->start();
                    break;
                }
                case AKS_Release: {
                    if (m_Private->m_LongPressTimer->isActive()) {
                        m_Private->m_Show = true;
                        g_Audio->requestDecreaseVolume();
                    }
                    m_Private->m_LongPressTimer->stop();
                    break;
                }
                default: {
                    break;
                }
                }
                break;
            }
            case AKT_CommonVolumeMute: {
                if ((AKS_Release == key->status)
                        || (AKS_Active == key->status)) {
                    m_Private->m_Show = true;
                    g_Audio->requestMuteToggole();
                }
                break;
            }
            default: {
                break;
            }
            }
        }
        break;
    }
    default: {
        break;
    }
    }
}

void VolumeWidget::onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status)
{
    if (WidgetStatus::Show == status) {
        if (isVisible()) {
            if (NULL !=  m_Private->m_Timer) {
                m_Private->m_Timer->stop();
            }
            setVisible(false);
        }
    }
}

void VolumeWidget::onMuteChange(const MuteItem mute)
{
    m_Private->initializeBackground();
    m_Private->initializeVolumeIcon();
    m_Private->initializeTextWidget();
    m_Private->initializeTimer();
    m_Private->m_Timer->start();
    QString volume("%1");
    if (MI_Unmute == mute) {
        m_Private->m_TextWidget->setText(volume.arg(m_Private->m_Volume, 2, 10, QChar('0')));
        m_Private->m_VolumeIcon->setBackgroundBmpPath(QString(":/Images/VolumeWidgetUnmuteIcon.png"));
    } else if (MI_Mute == mute) {
        m_Private->m_TextWidget->setText(volume.arg(0, 2, 10, QChar('0')));
        m_Private->m_VolumeIcon->setBackgroundBmpPath(QString(":/Images/VolumeWidgetMuteIcon.png"));
    }
    if ((m_Private->m_Show)
            && (!isVisible())) {
        raise();
        setVisible(true);
    }
    m_Private->m_Show = false;
}

void VolumeWidget::onVolumeChange(const int volume)
{
    m_Private->initializeBackground();
    m_Private->initializeVolumeIcon();
    m_Private->initializeTextWidget();
    m_Private->initializeTimer();
    m_Private->m_Timer->start();
    m_Private->m_Volume = volume;
    QString text("%1");
    m_Private->m_TextWidget->setText(text.arg(volume, 2, 10, QChar('0')));
    connectSignalAndSlotByNamesake(g_Widget, this, ARKRECEIVER(onWidgetTypeChange(const Widget::Type, const Widget::Type, const QString &)));
    raise();
    if ((m_Private->m_Show)
            && (!isVisible())) {
        raise();
        setVisible(true);
    }
    m_Private->m_Show = false;
}

void VolumeWidget::onTimeout()
{
    QTimer* ptr = static_cast<QTimer*>(sender());
    if (ptr == m_Private->m_Timer) {
        setVisible(false);
    } else if (ptr == m_Private->m_LongPressTimer) {
        if (MI_Unmute == g_Audio->getMute()) {
            m_Private->m_Show = true;
            g_Audio->requestMuteToggole();
        }
    }
}

VolumeWidgetPrivate::VolumeWidgetPrivate(VolumeWidget *parent)
    : m_Parent(parent)
{
    m_Timer = NULL;
    m_Background = NULL;
    m_VolumeIcon = NULL;
    m_TextWidget = NULL;
    m_Volume = 0;
    m_Show = false;
    m_LongPressTimer = NULL;
    initializeParent();
    connectAllSlots();
}

VolumeWidgetPrivate::~VolumeWidgetPrivate()
{
}

void VolumeWidgetPrivate::initializeParent()
{
    m_Parent->setVisible(false);
}

void VolumeWidgetPrivate::initializeBackground()
{
    if (NULL == m_Background) {
        m_Background = new BmpWidget(m_Parent);
        m_Background->setBackgroundBmpPath(QString(":/Images/VolumeWidgetBackground.png"));
        g_Widget->geometryFit(261, 179, 278, 122, m_Background);
        m_Background->setVisible(true);
    }
}

void VolumeWidgetPrivate::initializeVolumeIcon()
{
    if (NULL == m_VolumeIcon) {
        m_VolumeIcon = new BmpWidget(m_Background);
        m_VolumeIcon->setBackgroundBmpPath(QString(":/Images/VolumeWidgetUnmuteIcon.png"));
        g_Widget->geometryFit(26, 23, 86, 76, m_VolumeIcon);
        m_VolumeIcon->setVisible(true);
    }
}

void VolumeWidgetPrivate::initializeTextWidget()
{
    if (NULL == m_TextWidget) {
        m_TextWidget = new TextWidget(m_Background);
        m_TextWidget->setLanguageType(TextWidget::T_NoTranslate);
        m_TextWidget->setFontPointSize(g_Widget->widthScalabilityFactor() * 55);
        g_Widget->geometryFit(139, 0, 139, 122, m_TextWidget);
        m_TextWidget->setVisible(true);
    }
}

void VolumeWidgetPrivate::initializeTimer()
{
    if (NULL == m_Timer) {
        m_Timer = new QTimer(m_Parent);
        m_Timer->setSingleShot(true);
        m_Timer->setInterval(3000);
        Qt::ConnectionType type = static_cast<Qt::ConnectionType>(Qt::AutoConnection | Qt::UniqueConnection);
        QObject::connect(m_Timer,  ARKSENDER(timeout()),
                         m_Parent, ARKRECEIVER(onTimeout()),
                         type);
    }
}

void VolumeWidgetPrivate::initializeLongPressTimer()
{
    if (NULL == m_LongPressTimer) {
        m_LongPressTimer = new QTimer(m_Parent);
        m_LongPressTimer->setSingleShot(true);
        m_LongPressTimer->setInterval(1500);
        Qt::ConnectionType type = static_cast<Qt::ConnectionType>(Qt::AutoConnection | Qt::UniqueConnection);
        QObject::connect(m_LongPressTimer,  ARKSENDER(timeout()),
                         m_Parent,          ARKRECEIVER(onTimeout()),
                         type);
    }
}

void VolumeWidgetPrivate::connectAllSlots()
{
    connectSignalAndSlotByNamesake(g_Setting, m_Parent, ARKRECEIVER(onArkProtocol(const ArkProtocolWrapper&)));
}
