#include "DiskWidget.h"
#include "USBDiskWidget/USBDiskWidget.h"
#include "SDDiskWidget/SDDiskWidget.h"
#include "AutoConnect.h"
#include "MainWidget/DiskWidget/MusicWidget/MusicWidget.h"
#include "MainWidget/DiskWidget/ImageWidget/ImageWidget.h"
#include "MainWidget/DiskWidget/VideoWidget/VideoWidget.h"
#include "EventEngine.h"
#include "BusinessLogicUtility.h"

class DiskWidgetPrivate
{
    Q_DISABLE_COPY(DiskWidgetPrivate)
public:
    explicit DiskWidgetPrivate(DiskWidget* parent);
    ~DiskWidgetPrivate();
    void initializeUSBDiskWidget();
    void initializeSDDiskWidget();
    void initializeMusicWidget();
    void initializeImageWidget();
    void initializeVideoWidget();
    void connectAllSlots();
    USBDiskWidget* m_USBDiskWidget = NULL;
    SDDiskWidget* m_SDDiskWidget = NULL;
    MusicWidget* m_MusicWidget = NULL;
    ImageWidget* m_ImageWidget = NULL;
    VideoWidget* m_VideoWidget = NULL;
private:
    DiskWidget* m_Parent = NULL;
};

DiskWidget::DiskWidget(QWidget *parent)
    : QWidget(parent)
    , Widget::Interface()
    , m_Private(new DiskWidgetPrivate(this))
{
}

DiskWidget::~DiskWidget()
{
}

void DiskWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(0, 0, g_Widget->baseWindowWidth(), g_Widget->baseWindowHeight(), this);
}

void DiskWidget::ontWidgetTypeChange(const Widget::Type type, const QString &status)
{
    switch (type) {
    case Widget::T_SDDisk:
    case Widget::T_USBDisk:
    case Widget::T_Music:
    case Widget::T_Image:
    case Widget::T_ImageFullScreen:
    case Widget::T_Video:
    case Widget::T_VideoFullScreen: {
        if (WidgetStatus::Show == status) {
            lower();
            setVisible(true);
        }
        break;
    }
    case Widget::T_SDDiskMusic:
    case Widget::T_SDDiskImage:
    case Widget::T_SDDiskVideo:
    case Widget::T_USBDiskMusic:
    case Widget::T_USBDiskImage:
    case Widget::T_USBDiskVideo: {
        if (WidgetStatus::Show == status) {
            lower();
            setVisible(true);
        }
        break;
    }
    default: {
        if (WidgetStatus::Show == status) {
            setVisible(false);
        }
        break;
    }
    }
}

DiskWidgetPrivate::DiskWidgetPrivate(DiskWidget *parent)
    : m_Parent(parent)
{
    initializeSDDiskWidget();
    initializeUSBDiskWidget();
    initializeMusicWidget();
    initializeImageWidget();
    initializeVideoWidget();
    connectAllSlots();
}

DiskWidgetPrivate::~DiskWidgetPrivate()
{
}

void DiskWidgetPrivate::initializeUSBDiskWidget()
{
    if (NULL == m_USBDiskWidget) {
        m_USBDiskWidget = new USBDiskWidget(m_Parent);
    }
}

void DiskWidgetPrivate::initializeSDDiskWidget()
{
    if (NULL == m_SDDiskWidget) {
        m_SDDiskWidget = new SDDiskWidget(m_Parent);
    }
}

void DiskWidgetPrivate::initializeMusicWidget()
{
    if (NULL == m_MusicWidget) {
        m_MusicWidget = new MusicWidget(m_Parent);
    }
}

void DiskWidgetPrivate::initializeImageWidget()
{
    if (NULL == m_ImageWidget) {
        m_ImageWidget = new ImageWidget(m_Parent);
    }
}

void DiskWidgetPrivate::initializeVideoWidget()
{
    if (NULL == m_VideoWidget) {
        m_VideoWidget = new VideoWidget(m_Parent);
    }
}

void DiskWidgetPrivate::connectAllSlots()
{
    connectSignalAndSlotByNamesake(g_Widget, m_Parent);
}
