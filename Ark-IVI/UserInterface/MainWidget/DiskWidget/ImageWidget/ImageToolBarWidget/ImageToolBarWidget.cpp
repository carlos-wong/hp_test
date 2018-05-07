#include "ImageToolBarWidget.h"
#include "BmpButton.h"
#include "UserInterfaceUtility.h"
#include "AutoConnect.h"
#include "BmpWidget.h"
#include <QPainter>

class ImageToolBarWidgetPrivate
{
    Q_DISABLE_COPY(ImageToolBarWidgetPrivate)
public:
    explicit ImageToolBarWidgetPrivate(ImageToolBarWidget* parent);
    ~ImageToolBarWidgetPrivate();
    void initialize();
    void connectSlots();
    BmpWidget* m_Background;
    BmpButton* m_RotateBtn;
    BmpButton* m_PreviousBtn;
    BmpButton* m_ToggleBtn;
    BmpButton* m_NextBtn;
    BmpButton* m_ZoomInBtn;
    BmpButton* m_ZoomOutBtn;
private:
    ImageToolBarWidget* m_Parent;
};

ImageToolBarWidget::ImageToolBarWidget(QWidget *parent)
    : QWidget(parent)
    , m_Private(new ImageToolBarWidgetPrivate(this))
{
}

ImageToolBarWidget::~ImageToolBarWidget()
{
}

void ImageToolBarWidget::setDisabledZoom(const bool flag)
{
    m_Private->m_ZoomInBtn->setDisabled(flag);
    m_Private->m_ZoomOutBtn->setDisabled(flag);
}

void ImageToolBarWidget::setToolButtonStatus(const ImageToolBarWidget::PlayStatus status)
{
    switch (status) {
    case ImageToolBarWidget::PS_Play: {
        m_Private->m_ToggleBtn->setNormalBmpPath(QString(":/Images/hw_01_multimedia_pause_normal.png"));
        m_Private->m_ToggleBtn->setPressBmpPath(QString(":/Images/hw_01_multimedia_pause_pressed.png"));
        break;
    }
    case ImageToolBarWidget::PS_Pause: {
        m_Private->m_ToggleBtn->setNormalBmpPath(QString(":/Images/hw_01_multimedia_play_normal.png"));
        m_Private->m_ToggleBtn->setPressBmpPath(QString(":/Images/hw_01_multimedia_play_pressed.png"));
        break;
    }
    default: {
        break;
    }
    }
}

void ImageToolBarWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(0, 387, 800, 94, this);
}

void ImageToolBarWidget::onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status)
{
    switch (destinationType) {
    case Widget::T_SDImage:
    case Widget::T_USBImage: {
        if (WidgetStatus::Show == status) {
            setVisible(true);
        }
        break;
    }
    case Widget::T_SDImageFullScreen:
    case Widget::T_USBImageFullScreen: {
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

void ImageToolBarWidget::onToolButtonRelease()
{
    BmpButton* ptr = static_cast<BmpButton*>(sender());
    if (ptr == m_Private->m_RotateBtn) {
        toolButtonRelease(ImageToolBarWidget::BT_Rotate);
    } else if (ptr == m_Private->m_PreviousBtn) {
        toolButtonRelease(ImageToolBarWidget::BT_Previous);
    } else if (ptr == m_Private->m_ToggleBtn) {
        toolButtonRelease(ImageToolBarWidget::BT_Toggle);
    } else if (ptr == m_Private->m_NextBtn) {
        toolButtonRelease(ImageToolBarWidget::BT_Next);
    } else if (ptr == m_Private->m_ZoomInBtn) {
//        toolButtonRelease(ImageToolBarWidget::BT_ZoomIn);
        toolButtonRelease(ImageToolBarWidget::BT_ZoomOut);
    } else if (ptr == m_Private->m_ZoomOutBtn) {
//        toolButtonRelease(ImageToolBarWidget::BT_ZoomOut);
        toolButtonRelease(ImageToolBarWidget::BT_ZoomIn);
    }
}

ImageToolBarWidgetPrivate::ImageToolBarWidgetPrivate(ImageToolBarWidget *parent)
    : m_Parent(parent)
{
    m_Background = NULL;
    m_RotateBtn = NULL;
    m_PreviousBtn = NULL;
    m_ToggleBtn = NULL;
    m_NextBtn = NULL;
    m_ZoomInBtn = NULL;
    m_ZoomOutBtn = NULL;
    initialize();
    connectSlots();
}

ImageToolBarWidgetPrivate::~ImageToolBarWidgetPrivate()
{
}

void ImageToolBarWidgetPrivate::initialize()
{
    int btnY = 7;
    int btnWidth = 133;
    int btnHeight = 94;

    m_Background = new BmpWidget(m_Parent);
    m_Background->setBackgroundBmpPath(QString(":/Images/hw_01_image_bottom_toolbar.png"));
    g_Widget->geometryFit(0, btnY, 800, btnHeight, m_Background);
    m_RotateBtn = new BmpButton(m_Parent);
    g_Widget->geometryFit(0, btnY, btnWidth, btnHeight, m_RotateBtn);
    m_RotateBtn->setNormalBmpPath(QString(":/Images/hw_01_image_rotate_normal.png"));
    m_RotateBtn->setPressBmpPath(QString(":/Images/hw_01_image_rotate_pressed.png"));
    m_RotateBtn->setVisible(true);
    m_PreviousBtn = new BmpButton(m_Parent);
    g_Widget->geometryFit(btnWidth+2, btnY, btnWidth-1, btnHeight, m_PreviousBtn);
    m_PreviousBtn->setNormalBmpPath(QString(":/Images/hw_01_image_previous_normal.png"));
    m_PreviousBtn->setPressBmpPath(QString(":/Images/hw_01_image_previous_pressed.png"));
    m_PreviousBtn->setVisible(true);
    m_ToggleBtn = new BmpButton(m_Parent);
    m_ToggleBtn->setNormalBmpPath(QString(":/Images/hw_01_image_play_normal.png"));
    m_ToggleBtn->setPressBmpPath(QString(":/Images/hw_01_image_play_pressed.png"));
    g_Widget->geometryFit(btnWidth*2+2, btnY, btnWidth-1, btnHeight, m_ToggleBtn);
    m_ToggleBtn->setVisible(true);
    m_NextBtn = new BmpButton(m_Parent);
    g_Widget->geometryFit(btnWidth*3+3, btnY, btnWidth-1, btnHeight, m_NextBtn);
    m_NextBtn->setNormalBmpPath(QString(":/Images/hw_01_image_next_normal.png"));
    m_NextBtn->setPressBmpPath(QString(":/Images/hw_01_image_next_pressed.png"));
    m_NextBtn->setVisible(true);
    m_ZoomInBtn = new BmpButton(m_Parent);
    g_Widget->geometryFit(btnWidth*4+4, btnY, btnWidth-2, btnHeight, m_ZoomInBtn);
    m_ZoomInBtn->setNormalBmpPath(QString(":/Images/hw_01_image_zoom_in_normal.png"));
    m_ZoomInBtn->setPressBmpPath(QString(":/Images/hw_01_image_zoom_in_pressed.png"));
    m_ZoomInBtn->setVisible(true);
    m_Parent->setVisible(true);
    m_ZoomOutBtn = new BmpButton(m_Parent);
    g_Widget->geometryFit(btnWidth*5+3, btnY, btnWidth+2, btnHeight, m_ZoomOutBtn);
    m_ZoomOutBtn->setNormalBmpPath(QString(":/Images/hw_01_image_zoom_out_normal.png"));
    m_ZoomOutBtn->setPressBmpPath(QString(":/Images/hw_01_image_zoom_out_pressed.png"));
    m_ZoomOutBtn->setVisible(true);
}

void ImageToolBarWidgetPrivate::connectSlots()
{
    connectSignalAndSlotByNamesake(g_Widget, m_Parent, ARKRECEIVER(onWidgetTypeChange(const Widget::Type, const Widget::Type, const QString &)));
    Qt::ConnectionType type = static_cast<Qt::ConnectionType>(Qt::AutoConnection | Qt::UniqueConnection);
    QObject::connect(m_RotateBtn, ARKSENDER(released()),
                     m_Parent,    ARKRECEIVER(onToolButtonRelease()),
                     type);
    QObject::connect(m_PreviousBtn, ARKSENDER(released()),
                     m_Parent,      ARKRECEIVER(onToolButtonRelease()),
                     type);
    QObject::connect(m_ToggleBtn, ARKSENDER(released()),
                     m_Parent,    ARKRECEIVER(onToolButtonRelease()),
                     type);
    QObject::connect(m_NextBtn,   ARKSENDER(released()),
                     m_Parent,    ARKRECEIVER(onToolButtonRelease()),
                     type);
    QObject::connect(m_ZoomInBtn, ARKSENDER(released()),
                     m_Parent,    ARKRECEIVER(onToolButtonRelease()),
                     type);
    QObject::connect(m_ZoomOutBtn, ARKSENDER(released()),
                     m_Parent,     ARKRECEIVER(onToolButtonRelease()),
                     type);
}
