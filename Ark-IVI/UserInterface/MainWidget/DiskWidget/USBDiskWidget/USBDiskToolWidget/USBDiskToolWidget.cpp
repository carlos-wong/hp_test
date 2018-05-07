#include "USBDiskToolWidget.h"
#include "BmpButton.h"
#include "TextWidget.h"
#include "AutoConnect.h"
#include "UserInterfaceUtility.h"
#include <QPainter>

namespace SourceString {
static const QString Music(QObject::tr("Music"));
static const QString Image(QObject::tr("Image"));
static const QString Video(QObject::tr("Video"));
}

class USBDiskToolWidgetPrivate
{
    Q_DISABLE_COPY(USBDiskToolWidgetPrivate)
public:
    explicit USBDiskToolWidgetPrivate(USBDiskToolWidget* parent);
    ~USBDiskToolWidgetPrivate();
    void initialize();
    void connectAllSlots();
    BmpButton* m_MusicBtn;
    BmpButton* m_ImageBtn;
    BmpButton* m_VideoBtn;
    TextWidget* m_MusicText;
    TextWidget* m_ImageText;
    TextWidget* m_VideoText;
private:
    USBDiskToolWidget* m_Parent;
};

USBDiskToolWidget::USBDiskToolWidget(QWidget *parent)
    : QWidget(parent)
    , m_Private(new USBDiskToolWidgetPrivate(this))
{   
    setVisible(false);
}

USBDiskToolWidget::~USBDiskToolWidget()
{
}

void USBDiskToolWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(0, 0, 196, 428, this);
}

void USBDiskToolWidget::onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status)
{
    switch (destinationType) {
    case Widget::T_USBDiskMusic: {
        if (WidgetStatus::Show == status) {
            m_Private->m_MusicBtn->setStatus(BmpButton::B_Check);
            m_Private->m_ImageBtn->setStatus(BmpButton::B_Normal);
            m_Private->m_VideoBtn->setStatus(BmpButton::B_Normal);
            m_Private->m_MusicBtn->setVisible(true);
//            m_Private->m_MusicText->setVisible(true);
            m_Private->m_ImageBtn->setVisible(true);
//            m_Private->m_ImageText->setVisible(true);
            m_Private->m_VideoBtn->setVisible(true);
//            m_Private->m_VideoText->setVisible(true);
            setVisible(true);
        }
        break;
    }
    case Widget::T_USBDiskImage: {
        if (WidgetStatus::Show == status) {
            m_Private->m_MusicBtn->setStatus(BmpButton::B_Normal);
            m_Private->m_ImageBtn->setStatus(BmpButton::B_Check);
            m_Private->m_VideoBtn->setStatus(BmpButton::B_Normal);
            m_Private->m_MusicBtn->setVisible(true);
//            m_Private->m_MusicText->setVisible(true);
            m_Private->m_ImageBtn->setVisible(true);
//            m_Private->m_ImageText->setVisible(true);
            m_Private->m_VideoBtn->setVisible(true);
//            m_Private->m_VideoText->setVisible(true);
            setVisible(true);
        }
        break;
    }
    case Widget::T_USBDiskVideo: {
        if (WidgetStatus::Show == status) {
            m_Private->m_MusicBtn->setStatus(BmpButton::B_Normal);
            m_Private->m_ImageBtn->setStatus(BmpButton::B_Normal);
            m_Private->m_VideoBtn->setStatus(BmpButton::B_Check);
            m_Private->m_MusicBtn->setVisible(true);
//            m_Private->m_MusicText->setVisible(true);
            m_Private->m_ImageBtn->setVisible(true);
//            m_Private->m_ImageText->setVisible(true);
            m_Private->m_VideoBtn->setVisible(true);
//            m_Private->m_VideoText->setVisible(true);
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
    default: {
        break;
    }
    }
}

void USBDiskToolWidget::onDeviceWatcherStatus(const DeviceWatcherType type, const DeviceWatcherStatus status)
{
    qDebug() << "USBDiskWidget::onDeviceWatcherStatus" << type << status;
    if (DWT_USBDisk == type) {
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

void USBDiskToolWidget::onToolButtonRelease()
{
    BmpButton* ptr = static_cast<BmpButton*>(sender());
    if (ptr == m_Private->m_MusicBtn) {
        g_Widget->setWidgetType(Widget::T_USBDiskMusic, Widget::T_USBDisk, WidgetStatus::RequestShow);
    } else if (ptr == m_Private->m_ImageBtn) {
        g_Widget->setWidgetType(Widget::T_USBDiskImage, Widget::T_USBDisk, WidgetStatus::RequestShow);
    } else if (ptr == m_Private->m_VideoBtn) {
        g_Widget->setWidgetType(Widget::T_USBDiskVideo, Widget::T_USBDisk, WidgetStatus::RequestShow);
    }
}

USBDiskToolWidgetPrivate::USBDiskToolWidgetPrivate(USBDiskToolWidget *parent)
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

USBDiskToolWidgetPrivate::~USBDiskToolWidgetPrivate()
{
}

void USBDiskToolWidgetPrivate::initialize()
{
    m_MusicBtn = new BmpButton(m_Parent);
    m_MusicBtn->setNormalBmpPath(QString(":/Images/hw_01_multimedia_music_list_normal.png"));
    m_MusicBtn->setPressBmpPath(QString(":/Images/hw_01_multimedia_music_list_normal.png"));
    m_MusicBtn->setCheckBmpPath(QString(":/Images/hw_01_multimedia_music_list_checked.png"));
    g_Widget->geometryFit(23, 18, 170, 116, m_MusicBtn);
    m_MusicBtn->setVisible(true);
    m_VideoBtn = new BmpButton(m_Parent);
    m_VideoBtn->setNormalBmpPath(QString(":/Images/hw_01_multimedia_video_list_normal.png"));
    m_VideoBtn->setPressBmpPath(QString(":/Images/hw_01_multimedia_video_list_normal.png"));
    m_VideoBtn->setCheckBmpPath(QString(":/Images/hw_01_multimedia_video_list_checked.png"));
    g_Widget->geometryFit(23, 144, 170, 116, m_VideoBtn);
    m_ImageBtn = new BmpButton(m_Parent);
    m_ImageBtn->setNormalBmpPath(QString(":/Images/hw_01_multimedia_image_list_normal.png"));
    m_ImageBtn->setPressBmpPath(QString(":/Images/hw_01_multimedia_image_list_normal.png"));
    m_ImageBtn->setCheckBmpPath(QString(":/Images/hw_01_multimedia_image_list_checked.png"));
    g_Widget->geometryFit(23, 270, 170, 116, m_ImageBtn);
    m_ImageBtn->setVisible(true);
    m_VideoBtn->setVisible(true);
    m_MusicText = new TextWidget(m_MusicBtn);
    g_Widget->geometryFit(0, 61, 149, 47, m_MusicText);
    m_MusicText->setText(SourceString::Music);
    m_MusicText->setVisible(false);
    m_ImageText = new TextWidget(m_ImageBtn);
    g_Widget->geometryFit(0, 61, 149, 47, m_ImageText);
    m_ImageText->setText(SourceString::Image);
    m_ImageText->setVisible(false);
    m_VideoText = new TextWidget(m_VideoBtn);
    g_Widget->geometryFit(0, 61, 149, 47, m_VideoText);
    m_VideoText->setText(SourceString::Video);
    m_VideoText->setVisible(false);
}

void USBDiskToolWidgetPrivate::connectAllSlots()
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
