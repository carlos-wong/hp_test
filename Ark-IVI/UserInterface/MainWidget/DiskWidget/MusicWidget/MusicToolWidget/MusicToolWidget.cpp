#include "MusicToolWidget.h"
#include "BusinessLogic/Setting.h"
#include "BusinessLogic/BusinessLogicUtility.h"
#include "UserInterfaceUtility.h"
#include "BmpButton.h"
#include "BmpWidget.h"
#include "Slider.h"
#include "TextWidget.h"
#include "UserInterfaceUtility.h"
#include "BmpWidget.h"
#include <QPainter>
#include <QFileInfo>
#include <QVariant>
#include <QTimer>

class MusicToolWidgetPrivate
{
    Q_DISABLE_COPY(MusicToolWidgetPrivate)
public:
    explicit MusicToolWidgetPrivate(MusicToolWidget *parent);
    ~MusicToolWidgetPrivate();
    void initialize();
    void connectAllSlots();
    BmpWidget* m_Background;
    BmpButton* m_PreviousBtn;
    BmpButton* m_ToggleBtn;
    BmpButton* m_NextBtn;
    BmpButton* m_ModeBtn;
    BmpButton* m_ListBtn;
private:
    MusicToolWidget* m_Parent;
};

MusicToolWidget::MusicToolWidget(QWidget *parent)
    : QWidget(parent)
    , m_Private(new MusicToolWidgetPrivate(this))
{
    setVisible(true);
}

MusicToolWidget::~MusicToolWidget()
{
}

void MusicToolWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(0, 387, 800, 94, this);
}

void MusicToolWidget::onMusicPlayerPlayMode(const MusicPlayerPlayMode mode)
{
    switch (mode) {
    case MPPM_RepeatOnce: {
        m_Private->m_ModeBtn->setNormalBmpPath(QString(":/Images/MultimediaSingleNormal.png"));
        m_Private->m_ModeBtn->setPressBmpPath(QString(":/Images/MultimediaSinglePress.png"));
        break;
    }
    case MPPM_Shuffle: {
        m_Private->m_ModeBtn->setNormalBmpPath(QString(":/Images/MultimediaRandomNormal.png"));
        m_Private->m_ModeBtn->setPressBmpPath(QString(":/Images/MultimediaRandomPress.png"));
        break;
    }
    case MPPM_AllRepeat:
    default : {
        m_Private->m_ModeBtn->setNormalBmpPath(QString(":/Images/MultimediaAllNormal.png"));
        m_Private->m_ModeBtn->setPressBmpPath(QString(":/Images/MultimediaAllPress.png"));
        break;
    }
    }
}

void MusicToolWidget::onMusicPlayerPlayStatus(const DeviceWatcherType type, const MusicPlayerPlayStatus status)
{
    qDebug() << "MusicToolWidget::onMusicPlayerPlayStatus" << status;
    m_Private->m_PreviousBtn->setEnabled(MPPS_Start != status);
    m_Private->m_ToggleBtn->setEnabled(MPPS_Start != status);
    m_Private->m_NextBtn->setEnabled(MPPS_Start != status);
    switch (status) {
    case MPPS_Play: {
        m_Private->m_ToggleBtn->setNormalBmpPath(QString(":/Images/MultimediaPauseNormal.png"));
        m_Private->m_ToggleBtn->setPressBmpPath(QString(":/Images/MultimediaPausePress.png"));
        break;
    }
    case MPPS_Start:
    case MPPS_Unsupport:
    case MPPS_Pause:
    case MPPS_Stop: {
        m_Private->m_ToggleBtn->setNormalBmpPath(QString(":/Images/MultimediaPlayNormal.png"));
        m_Private->m_ToggleBtn->setPressBmpPath(QString(":/Images/MultimediaPlayPress.png"));
        break;
    }
    default : {
        break;
    }
    }
}

MusicToolWidgetPrivate::MusicToolWidgetPrivate(MusicToolWidget *parent)
    : m_Parent(parent)
{
    m_Background = NULL;
    m_PreviousBtn = NULL;
    m_ToggleBtn = NULL;
    m_NextBtn = NULL;
    m_ModeBtn = NULL;
    m_ListBtn = NULL;
    initialize();
    connectAllSlots();
}

MusicToolWidgetPrivate::~MusicToolWidgetPrivate()
{
}

void MusicToolWidgetPrivate::initialize()
{
    m_Background = new BmpWidget(m_Parent);
    m_Background->setBackgroundBmpPath(QString(":/Images/MultimediaToolWidgetBackground.png"));
    g_Widget->geometryFit(0, 0, 800, 94, m_Background);
    m_Background->setVisible(true);
    m_PreviousBtn = new BmpButton(m_Parent);
    g_Widget->geometryFit(0, 14, 160, 80, m_PreviousBtn);
    m_PreviousBtn->setNormalBmpPath(QString(":/Images/MultimediaPreviousNormal.png"));
    m_PreviousBtn->setPressBmpPath(QString(":/Images/MultimediaPreviousPress.png"));
    m_PreviousBtn->setVisible(true);
    m_ToggleBtn = new BmpButton(m_Parent);
    g_Widget->geometryFit(160, 14, 160, 80, m_ToggleBtn);
    m_ToggleBtn->setNormalBmpPath(QString(":/Images/MultimediaPlayNormal.png"));
    m_ToggleBtn->setPressBmpPath(QString(":/Images/MultimediaPlayPress.png"));
    m_ToggleBtn->setVisible(true);
    m_NextBtn = new BmpButton(m_Parent);
    g_Widget->geometryFit(320, 14, 160, 80, m_NextBtn);
    m_NextBtn->setNormalBmpPath(QString(":/Images/MultimediaNextNormal.png"));
    m_NextBtn->setPressBmpPath(QString(":/Images/MultimediaNextPress.png"));
    m_NextBtn->setVisible(true);
    m_ModeBtn = new BmpButton(m_Parent);
    g_Widget->geometryFit(480, 14, 160, 80, m_ModeBtn);
    m_ModeBtn->setNormalBmpPath(QString(":/Images/MultimediaAllNormal.png"));
    m_ModeBtn->setPressBmpPath(QString(":/Images/MultimediaAllPress.png"));
    m_ModeBtn->setVisible(true);
    m_ListBtn = new BmpButton(m_Parent);
    g_Widget->geometryFit(640, 14, 160, 80, m_ListBtn);
    m_ListBtn->setNormalBmpPath(QString(":/Images/MultimediaListNormal.png"));
    m_ListBtn->setPressBmpPath(QString(":/Images/MultimediaListPress.png"));
    m_ListBtn->setVisible(true);
}

void MusicToolWidgetPrivate::connectAllSlots()
{
    connectSignalAndSlotByNamesake(g_Multimedia, m_Parent, ARKRECEIVER(onMusicPlayerPlayMode(const int)));
    connectSignalAndSlotByNamesake(g_Multimedia, m_Parent, ARKRECEIVER(onMusicPlayerPlayStatus(const int, const int)));
    Qt::ConnectionType type = static_cast<Qt::ConnectionType>(Qt::UniqueConnection | Qt::AutoConnection);
    QObject::connect(m_PreviousBtn, ARKSENDER(released()),
                     m_Parent,      ARKRECEIVER(onToolButtonRelease()),
                     type);
    QObject::connect(m_ToggleBtn, ARKSENDER(released()),
                     m_Parent,    ARKRECEIVER(onToolButtonRelease()),
                     type);
    QObject::connect(m_NextBtn, ARKSENDER(released()),
                     m_Parent,  ARKRECEIVER(onToolButtonRelease()),
                     type);
    QObject::connect(m_ModeBtn, ARKSENDER(released()),
                     m_Parent,  ARKRECEIVER(onToolButtonRelease()),
                     type);
    QObject::connect(m_ListBtn, ARKSENDER(released()),
                     m_Parent,  ARKRECEIVER(onToolButtonRelease()),
                     type);
}

void MusicToolWidget::onToolButtonRelease()
{
    BmpButton* ptr = static_cast<BmpButton*>(sender());
    if (ptr == m_Private->m_PreviousBtn) {
        g_Multimedia->musicPlayerPlayPreviousListViewIndex();
    } else if (ptr == m_Private->m_ToggleBtn) {
        g_Multimedia->musicPlayerSetPlayStatusToggle();
    } else if (ptr == m_Private->m_NextBtn) {
        g_Multimedia->musicPlayerPlayNextListViewIndex();
    } else if (ptr == m_Private->m_ModeBtn) {
        g_Multimedia->musicPlayerSetPlayModeToggle();
    } else if (ptr == m_Private->m_ListBtn) {
        g_Widget->setWidgetType(Widget::T_Back, Widget::T_Undefine, WidgetStatus::RequestShow);
    }
}
