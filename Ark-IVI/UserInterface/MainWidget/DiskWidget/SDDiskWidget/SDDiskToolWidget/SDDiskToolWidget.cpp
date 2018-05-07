#include "SDDiskToolWidget.h"
#include "UserInterfaceUtility.h"
#include "BmpButton.h"
#include "TextWidget.h"
#include "AutoConnect.h"
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
    setVisible(false);
}

SDDiskToolWidget::~SDDiskToolWidget()
{
}

void SDDiskToolWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(0, 0, 196, 428, this);
}

void SDDiskToolWidget::onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status)
{
    switch (destinationType) {
    case Widget::T_SDDiskMusic: {
        if (WidgetStatus::Show == status) {
            m_Private->m_MusicBtn->setStatus(BmpButton::B_Check);
            m_Private->m_ImageBtn->setStatus(BmpButton::B_Normal);
            m_Private->m_VideoBtn->setStatus(BmpButton::B_Normal);
            m_Private->m_MusicBtn->setVisible(true);
            m_Private->m_MusicText->setVisible(true);
            m_Private->m_ImageBtn->setVisible(true);
            m_Private->m_ImageText->setVisible(true);
            m_Private->m_VideoBtn->setVisible(true);
            m_Private->m_VideoText->setVisible(true);
            setVisible(true);
        }
        break;
    }
    case Widget::T_SDDiskImage: {
        if (WidgetStatus::Show == status) {
            m_Private->m_MusicBtn->setStatus(BmpButton::B_Normal);
            m_Private->m_ImageBtn->setStatus(BmpButton::B_Check);
            m_Private->m_VideoBtn->setStatus(BmpButton::B_Normal);
            m_Private->m_MusicBtn->setVisible(true);
            m_Private->m_MusicText->setVisible(true);
            m_Private->m_ImageBtn->setVisible(true);
            m_Private->m_ImageText->setVisible(true);
            m_Private->m_VideoBtn->setVisible(true);
            m_Private->m_VideoText->setVisible(true);
            setVisible(true);
        }
        break;
    }
    case Widget::T_SDDiskVideo: {
        if (WidgetStatus::Show == status) {
            m_Private->m_MusicBtn->setStatus(BmpButton::B_Normal);
            m_Private->m_ImageBtn->setStatus(BmpButton::B_Normal);
            m_Private->m_VideoBtn->setStatus(BmpButton::B_Check);
            m_Private->m_MusicBtn->setVisible(true);
            m_Private->m_MusicText->setVisible(true);
            m_Private->m_ImageBtn->setVisible(true);
            m_Private->m_ImageText->setVisible(true);
            m_Private->m_VideoBtn->setVisible(true);
            m_Private->m_VideoText->setVisible(true);
            setVisible(true);
        }
        break;
    }
        //    case Widget::T_Music:
        //    case Widget::T_Image:
        //    case Widget::T_Video: {
        //        if (WidgetStatus::Show == status) {
        //            setVisible(false);
        //        }
        //        break;
        //    }
        //    case Widget::T_Home: {
        //        if (WidgetStatus::Show == status) {
        //            setVisible(false);
        //        }
        //        break;
        //    }
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
            setVisible(false);
            m_Private->m_MusicBtn->setVisible(false);
            m_Private->m_MusicText->setVisible(false);
            m_Private->m_ImageBtn->setVisible(false);
            m_Private->m_ImageText->setVisible(false);
            m_Private->m_VideoBtn->setVisible(false);
            m_Private->m_VideoText->setVisible(false);
            break;
        }
        case DWS_Busy: {
            setVisible(false);
            m_Private->m_MusicBtn->setVisible(false);
            m_Private->m_MusicText->setVisible(false);
            m_Private->m_ImageBtn->setVisible(false);
            m_Private->m_ImageText->setVisible(false);
            m_Private->m_VideoBtn->setVisible(false);
            m_Private->m_VideoText->setVisible(false);
            break;
        }
        case DWS_Ready: {
            //            m_Private->m_MusicBtn->setVisible(true);
            //            m_Private->m_MusicText->setVisible(true);
            //            m_Private->m_ImageBtn->setVisible(true);
            //            m_Private->m_ImageText->setVisible(true);
            //            m_Private->m_VideoBtn->setVisible(true);
            //            m_Private->m_VideoText->setVisible(true);
            break;
        }
        case DWS_Remove: {
            setVisible(false);
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
    BmpButton* ptr = static_cast<BmpButton*>(sender());
    if (ptr == m_Private->m_MusicBtn) {
        g_Widget->setWidgetType(Widget::T_SDDiskMusic, Widget::T_SDDisk, WidgetStatus::RequestShow);
    } else if (ptr == m_Private->m_ImageBtn) {
        g_Widget->setWidgetType(Widget::T_SDDiskImage, Widget::T_SDDisk, WidgetStatus::RequestShow);
    } else if (ptr == m_Private->m_VideoBtn) {
        g_Widget->setWidgetType(Widget::T_SDDiskVideo, Widget::T_SDDisk, WidgetStatus::RequestShow);
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
    m_MusicBtn->setNormalBmpPath(QString(":/Images/MultimediaToolWidgetIMusicNormal.png"));
    m_MusicBtn->setPressBmpPath(QString(":/Images/MultimediaToolWidgetIMusicNormal.png"));
    m_MusicBtn->setCheckBmpPath(QString(":/Images/MultimediaToolWidgetIMusicPress.png"));
    g_Widget->geometryFit(23, 18, 170, 116, m_MusicBtn);
    m_MusicBtn->setVisible(true);

    m_ImageBtn = new BmpButton(m_Parent);
    m_ImageBtn->setNormalBmpPath(QString(":/Images/MultimediaToolWidgetImageNormal.png"));
    m_ImageBtn->setPressBmpPath(QString(":/Images/MultimediaToolWidgetImageNormal.png"));
    m_ImageBtn->setCheckBmpPath(QString(":/Images/MultimediaToolWidgetImagePress.png"));
    g_Widget->geometryFit(23, 144, 170, 116, m_ImageBtn);
    m_ImageBtn->setVisible(true);

    m_VideoBtn = new BmpButton(m_Parent);
    m_VideoBtn->setNormalBmpPath(QString(":/Images/MultimediaToolWidgetIVideoNormal.png"));
    m_VideoBtn->setPressBmpPath(QString(":/Images/MultimediaToolWidgetIVideoNormal.png"));
    m_VideoBtn->setCheckBmpPath(QString(":/Images/MultimediaToolWidgetIVideoPress.png"));
    g_Widget->geometryFit(23, 270, 170, 116, m_VideoBtn);
    m_VideoBtn->setVisible(true);

    m_MusicText = new TextWidget(m_MusicBtn);
    g_Widget->geometryFit(0, 61, 149, 47, m_MusicText);
    m_MusicText->setText(SourceString::Music);
    m_MusicText->setVisible(true);
    m_ImageText = new TextWidget(m_ImageBtn);
    g_Widget->geometryFit(0, 61, 149, 47, m_ImageText);
    m_ImageText->setText(SourceString::Image);
    m_ImageText->setVisible(true);
    m_VideoText = new TextWidget(m_VideoBtn);
    g_Widget->geometryFit(0, 61, 149, 47, m_VideoText);
    m_VideoText->setText(SourceString::Video);
    m_VideoText->setVisible(true);
}

void SDDiskToolWidgetPrivate::connectAllSlots()
{
    connectSignalAndSlotByNamesake(g_Widget, m_Parent, ARKRECEIVER(onWidgetTypeChange(const Widget::Type, const Widget::Type, const QString &)));
    connectSignalAndSlotByNamesake(g_Multimedia, m_Parent, ARKRECEIVER(onDeviceWatcherStatus(const int, const int)));
    Qt::ConnectionType type = static_cast<Qt::ConnectionType>(Qt::UniqueConnection | Qt::AutoConnection);
    QObject::connect(m_MusicBtn, ARKSENDER(released()),
                     m_Parent,   ARKRECEIVER(onToolButtonRelease()),
                     type);
    QObject::connect(m_ImageBtn, ARKSENDER(released()),
                     m_Parent,   ARKRECEIVER(onToolButtonRelease()),
                     type);
    QObject::connect(m_VideoBtn, ARKSENDER(released()),
                     m_Parent,   ARKRECEIVER(onToolButtonRelease()),
                     type);
}
