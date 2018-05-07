#include "MusicMiniWidget.h"
#include "BmpWidget.h"
#include "BmpButton.h"
#include "AutoConnect.h"
#include "BusinessLogic/Audio.h"
#include <QPainter>
#include <QDebug>
#include "TextWidget.h"

class MusicMiniWidgetPrivate
{
public:
    MusicMiniWidgetPrivate(MusicMiniWidget* parent);
    ~MusicMiniWidgetPrivate();
    void initializeParent();
    void initializeBasic();
    void connectAllSlots();
private:
    Q_DECLARE_PUBLIC(MusicMiniWidget)
    MusicMiniWidget* const q_ptr;
    BmpButton* m_Previous;
    BmpButton* m_Toggle;
    BmpButton* m_Next;
    BmpWidget *m_MusicLogo;
    TextWidget *m_MusicName;
    TextWidget *m_AuthorName;
};

MusicMiniWidget::MusicMiniWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new MusicMiniWidgetPrivate(this))
{
}

MusicMiniWidget::~MusicMiniWidget()
{
    if (NULL != d_ptr) {
        delete d_ptr;
    }
}

void MusicMiniWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(40, 80, 380, 150, this);
}

void MusicMiniWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.drawPixmap(this->rect(), QPixmap(":/Images/hw_01_big_box_home.png"));
}

void MusicMiniWidget::onMusicPlayerPlayStatus(const DeviceWatcherType type, const MusicPlayerPlayStatus status)
{
    Q_D(MusicMiniWidget);
    switch (status) {
    case MPPS_Play: {
        d->m_Toggle->setNormalBmpPath(QString(":/Images/hw_01_pause_mili_normal.png"));
        d->m_Toggle->setPressBmpPath(QString(":/Images/hw_01_pause_mili_pressed.png"));
        break;
    }
    case MPPS_Start:
    case MPPS_Pause:
    case MPPS_Stop: {
        d->m_Toggle->setNormalBmpPath(QString(":/Images/hw_01_play_mili_normal.png"));
        d->m_Toggle->setPressBmpPath(QString(":/Images/hw_01_play_mili_pressed.png"));
        break;
    }
    default : {
        break;
    }
    }
}

void MusicMiniWidget::onMusicPlayerID3TagChange(const int type, const int index, const QString &fileName,
                                                const QString &title, const QString &artist, const QString &album, const int endTime)
{
    Q_D(MusicMiniWidget);
    if (fileName.contains(".")) {
        QString curName = fileName;
        curName = curName.mid(0,curName.lastIndexOf("."));
        d->m_MusicName->setText(curName);
    } else {
        if (fileName.isEmpty()) {
            d->m_MusicName->setText(QString("Unknown"));
        } else {
            d->m_MusicName->setText(fileName);
        }
    }

    if (artist.isEmpty()) {
        d->m_AuthorName->setText(QString("Unknown"));
    } else {
        d->m_AuthorName->setText(artist);
    }

}

void MusicMiniWidget::onBmpButtonRelease()
{
    qDebug() << __PRETTY_FUNCTION__;
    BmpButton* ptr = static_cast<BmpButton*>(sender());
    Q_D(MusicMiniWidget);
    if (ptr == d->m_Previous) {
        g_Multimedia->musicPlayerPlayPreviousListViewIndex();
    } else if (ptr == d->m_Toggle) {
        g_Multimedia->musicPlayerSetPlayStatusToggle();
    } else if (ptr == d->m_Next) {
        g_Multimedia->musicPlayerPlayNextListViewIndex();
    }
}

MusicMiniWidgetPrivate::MusicMiniWidgetPrivate(MusicMiniWidget *parent)
    : q_ptr(parent)
{
    m_Previous = NULL;
    m_Toggle = NULL;
    m_Next = NULL;
    m_MusicLogo = NULL;
    m_MusicName = NULL;
    m_AuthorName = NULL;
    initializeBasic();
    initializeParent();
    connectAllSlots();
}

MusicMiniWidgetPrivate::~MusicMiniWidgetPrivate()
{
}

void MusicMiniWidgetPrivate::initializeParent()
{
    Q_Q(MusicMiniWidget);
    q->setVisible(false);
}

void MusicMiniWidgetPrivate::initializeBasic()
{
    Q_Q(MusicMiniWidget);

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
    g_Widget->geometryFit(30, 20, 75,75, m_MusicLogo);
    m_MusicLogo->setBackgroundBmpPath(":/Images/hw_01_music_mili_logol.png");

    m_MusicName = new TextWidget(q);
    g_Widget->geometryFit(120, 20, 225, 40, m_MusicName);
    m_MusicName->setLanguageType(TextWidget::T_NoTranslate);
    m_MusicName->setAlignmentFlag(Qt::AlignLeft | Qt::AlignVCenter);
    m_MusicName->setFontPointSize(12 * g_Widget->widthScalabilityFactor());
    m_MusicName->setAnimation(true);
    m_MusicName->setText(QString("Unknown"));

    m_AuthorName = new TextWidget(q);
    g_Widget->geometryFit(120, 60, 130,30, m_AuthorName);
    m_AuthorName->setLanguageType(TextWidget::T_NoTranslate);
    m_AuthorName->setAlignmentFlag(Qt::AlignLeft | Qt::AlignVCenter);
    m_AuthorName->setFontPointSize(8 * g_Widget->widthScalabilityFactor());
    m_AuthorName->setText(QString("Unknown"));
}

void MusicMiniWidgetPrivate::connectAllSlots()
{
    Q_Q(MusicMiniWidget);
    connectSignalAndSlotByNamesake(g_Multimedia, q, ARKRECEIVER(onMusicPlayerPlayStatus(const int, const int)));
    connectSignalAndSlotByNamesake(g_Multimedia, q, ARKRECEIVER(onMusicPlayerID3TagChange(const int, const int, const QString &, const QString&, const QString&, const QString&, const int)));
}
