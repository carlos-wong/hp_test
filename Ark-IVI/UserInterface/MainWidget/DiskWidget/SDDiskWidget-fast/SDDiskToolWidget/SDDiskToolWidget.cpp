#include "SDDiskToolWidget.h"
#include "BmpButton.h"
#include "TextWidget.h"
#include "AutoConnect.h"
#include <QDebug>
#include <QPainter>

namespace SourceString {
static const QString Music = QString(QObject::tr("Music"));
static const QString Image = QString(QObject::tr("Image"));
static const QString Video = QString(QObject::tr("Video"));
}

class SDDiskToolWidgetPrivate
{
    Q_DISABLE_COPY(SDDiskToolWidgetPrivate)
public:
    explicit SDDiskToolWidgetPrivate(SDDiskToolWidget* parent);
    ~SDDiskToolWidgetPrivate();
    void initialize();
    void connectAllSlots();
    BmpButton* m_MusicBtn;
    BmpButton* m_ImageBtn;
    BmpButton* m_VideoBtn;
    TextWidget* m_MusicText;
    TextWidget* m_ImageText;
    TextWidget* m_VideoText;
private:
    SDDiskToolWidget* m_Parent;
};

SDDiskToolWidget::SDDiskToolWidget(QWidget *parent)
    : QWidget(parent)
    , m_Private(new SDDiskToolWidgetPrivate(this))
{   
}

SDDiskToolWidget::~SDDiskToolWidget()
{
}

void SDDiskToolWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(0, 0, 264, g_Widget->baseWindowHeight(), this);
    g_Widget->geometryFit(0, 73, 264, 248 - 73, m_Private->m_MusicBtn);
    int width(100);
    int height(248 - 73);
    g_Widget->geometryFit(128 + (196 - 128 - width) * 0.5, 0, width, height, m_Private->m_MusicText);
    g_Widget->geometryFit(0, 262, 264, 437 - 262, m_Private->m_ImageBtn);
    g_Widget->geometryFit(128 + (196 - 128 - width) * 0.5, 0, width, height, m_Private->m_ImageText);
    g_Widget->geometryFit(0, 451, 264, 626 - 451, m_Private->m_VideoBtn);
    g_Widget->geometryFit(128 + (196 - 128 - width) * 0.5, 0, width, height, m_Private->m_VideoText);
    QWidget::resizeEvent(event);
}

void SDDiskToolWidget::paintEvent(QPaintEvent *event)
{
#ifdef gcc
    QPainter painter(this);
    painter.setPen(Qt::green);
    painter.drawRect(rect().adjusted(0, 0, -1, -1));
#endif
    QWidget::paintEvent(event);
}

void SDDiskToolWidget::ontWidgetTypeChange(const Widget::Type type, const QString &status)
{
    qDebug() << "SDDiskWidget::ontWidgetTypeChange" << type << status;
    switch (type) {
    case Widget::T_SDDisk: {
        if (WidgetStatus::Show == status) {
            setVisible(true);
        }
        break;
    }
    case Widget::T_SDDiskMusic: {
        if (WidgetStatus::Show == status) {
            m_Private->m_MusicBtn->setStatus(BmpButton::B_Check);
            m_Private->m_ImageBtn->setStatus(BmpButton::B_Normal);
            m_Private->m_VideoBtn->setStatus(BmpButton::B_Normal);
            setVisible(true);
        }
        break;
    }
    case Widget::T_SDDiskImage: {
        if (WidgetStatus::Show == status) {
            m_Private->m_MusicBtn->setStatus(BmpButton::B_Normal);
            m_Private->m_ImageBtn->setStatus(BmpButton::B_Check);
            m_Private->m_VideoBtn->setStatus(BmpButton::B_Normal);
            setVisible(true);
        }
        break;
    }
    case Widget::T_SDDiskVideo: {
        if (WidgetStatus::Show == status) {
            m_Private->m_MusicBtn->setStatus(BmpButton::B_Normal);
            m_Private->m_ImageBtn->setStatus(BmpButton::B_Normal);
            m_Private->m_VideoBtn->setStatus(BmpButton::B_Check);
            setVisible(true);
        }
        break;
    }
    case Widget::T_Music:
    case Widget::T_Image:
    case Widget::T_Video: {
        if (WidgetStatus::Show == status) {
            setVisible(false);
        }
        break;
    }
    case Widget::T_Home: {
        if (WidgetStatus::Show == status) {
            setVisible(false);
        }
        break;
    }
    default: {
        break;
    }
    }
}

void SDDiskToolWidget::onDeviceWatcherStatus(const DeviceWatcherType type, const DeviceWatcherStatus status)
{
    qDebug() << "SDDiskWidget::onDeviceWatcherStatus" << type << status;
    if (DWT_SDDisk == type) {
        switch (status) {
        case DWS_Empty: {
            m_Private->m_MusicBtn->setVisible(false);
            m_Private->m_MusicText->setVisible(false);
            m_Private->m_ImageBtn->setVisible(false);
            m_Private->m_ImageText->setVisible(false);
            m_Private->m_VideoBtn->setVisible(false);
            m_Private->m_VideoText->setVisible(false);
            break;
        }
        case DWS_Busy: {
            m_Private->m_MusicBtn->setVisible(false);
            m_Private->m_MusicText->setVisible(false);
            m_Private->m_ImageBtn->setVisible(false);
            m_Private->m_ImageText->setVisible(false);
            m_Private->m_VideoBtn->setVisible(false);
            m_Private->m_VideoText->setVisible(false);
            break;
        }
        case DWS_Ready: {
            m_Private->m_MusicBtn->setStatus(BmpButton::B_Check);
            m_Private->m_ImageBtn->setStatus(BmpButton::B_Normal);
            m_Private->m_VideoBtn->setStatus(BmpButton::B_Normal);
            m_Private->m_MusicBtn->setVisible(true);
            m_Private->m_MusicText->setVisible(true);
            m_Private->m_ImageBtn->setVisible(true);
            m_Private->m_ImageText->setVisible(true);
            m_Private->m_VideoBtn->setVisible(true);
            m_Private->m_VideoText->setVisible(true);
            break;
        }
        case DWS_Remove: {
            m_Private->m_MusicBtn->setVisible(false);
            m_Private->m_MusicText->setVisible(false);
            m_Private->m_ImageBtn->setVisible(false);
            m_Private->m_ImageText->setVisible(false);
            m_Private->m_VideoBtn->setVisible(false);
            m_Private->m_VideoText->setVisible(false);
            break;
        }
        default: {
            break;
        }
        }
    }
}

void SDDiskToolWidget::onToolButtonRelease()
{
    if (sender() == m_Private->m_MusicBtn) {
        g_Widget->setWidgetType(Widget::T_SDDiskMusic, WidgetStatus::RequestShow);
    } else if (sender() == m_Private->m_ImageBtn) {
        g_Widget->setWidgetType(Widget::T_SDDiskImage, WidgetStatus::RequestShow);
    } else if (sender() == m_Private->m_VideoBtn) {
        g_Widget->setWidgetType(Widget::T_SDDiskVideo, WidgetStatus::RequestShow);
    }
}

SDDiskToolWidgetPrivate::SDDiskToolWidgetPrivate(SDDiskToolWidget *parent)
    : m_Parent(parent)
{
    m_MusicBtn = NULL;
    m_ImageBtn = NULL;
    m_VideoBtn = NULL;
    m_MusicText = NULL;
    m_ImageText = NULL;
    m_VideoText = NULL;
    initialize();
    connectAllSlots();
}

SDDiskToolWidgetPrivate::~SDDiskToolWidgetPrivate()
{
}

void SDDiskToolWidgetPrivate::initialize()
{
    m_MusicBtn = new BmpButton(m_Parent);
    m_MusicBtn->hide();
    m_MusicBtn->setNormalBmpPath(QString(":/Images/Resources/Images/MultimediaWidgetMusicBtnNormal"));
    m_MusicBtn->setPressBmpPath(QString(":/Images/Resources/Images/MultimediaWidgetMusicBtnNormal"));
    m_MusicBtn->setCheckBmpPath(QString(":/Images/Resources/Images/MultimediaWidgetMusicBtnPress"));
    m_ImageBtn = new BmpButton(m_Parent);
    m_ImageBtn->hide();
    m_ImageBtn->setNormalBmpPath(QString(":/Images/Resources/Images/MultimediaWidgetImageBtnNormal"));
    m_ImageBtn->setPressBmpPath(QString(":/Images/Resources/Images/MultimediaWidgetImageBtnNormal"));
    m_ImageBtn->setCheckBmpPath(QString(":/Images/Resources/Images/MultimediaWidgetImageBtnPress"));
    m_VideoBtn = new BmpButton(m_Parent);
    m_VideoBtn->hide();
    m_VideoBtn->setNormalBmpPath(QString(":/Images/Resources/Images/MultimediaWidgetVideoBtnNormal"));
    m_VideoBtn->setPressBmpPath(QString(":/Images/Resources/Images/MultimediaWidgetVideoBtnNormal"));
    m_VideoBtn->setCheckBmpPath(QString(":/Images/Resources/Images/MultimediaWidgetVideoBtnPress"));
    int fontPointSize(22 * g_Widget->widthScalabilityFactor());
    m_MusicText = new TextWidget(m_MusicBtn);
    m_MusicText->hide();
    m_MusicText->setText(SourceString::Music);
    m_MusicText->setFontPointSize(fontPointSize);
    m_ImageText = new TextWidget(m_ImageBtn);
    m_ImageText->hide();
    m_ImageText->setText(SourceString::Image);
    m_ImageText->setFontPointSize(fontPointSize);
    m_VideoText = new TextWidget(m_VideoBtn);
    m_VideoText->hide();
    m_VideoText->setText(SourceString::Video);
    m_VideoText->setFontPointSize(fontPointSize);
}

void SDDiskToolWidgetPrivate::connectAllSlots()
{
    connectSignalAndSlotByNamesake(g_Widget, m_Parent);
    connectSignalAndSlotByNamesake(g_Multimedia, m_Parent);
    Qt::ConnectionType type = static_cast<Qt::ConnectionType>(Qt::UniqueConnection | Qt::AutoConnection);
    QObject::connect(m_MusicBtn, SIGNAL(bmpButtonRelease()),
                     m_Parent,   SLOT(onToolButtonRelease()),
                     type);
    QObject::connect(m_ImageBtn, SIGNAL(bmpButtonRelease()),
                     m_Parent,   SLOT(onToolButtonRelease()),
                     type);
    QObject::connect(m_VideoBtn, SIGNAL(bmpButtonRelease()),
                     m_Parent,   SLOT(onToolButtonRelease()),
                     type);
}
