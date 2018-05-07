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
        m_Private->m_ToggleBtn->setNormalBmpPath(QString(":/Images/ImagePauseNormal.png"));
        m_Private->m_ToggleBtn->setPressBmpPath(QString(":/Images/ImagePausePress.png"));
        break;
    }
    case ImageToolBarWidget::PS_Pause: {
        m_Private->m_ToggleBtn->setNormalBmpPath(QString(":/Images/ImagePlayNormal.png"));
        m_Private->m_ToggleBtn->setPressBmpPath(QString(":/Images/ImagePlayPress.png"));
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
        toolButtonRelease(ImageToolBarWidget::BT_ZoomIn);
    } else if (ptr == m_Private->m_ZoomOutBtn) {
        toolButtonRelease(ImageToolBarWidget::BT_ZoomOut);
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
    m_Background = new BmpWidget(m_Parent);
    m_Background->setBackgroundBmpPath(QString(":/Images/MultimediaToolWidgetBackground.png"));
    g_Widget->geometryFit(0, 0, 800, 94, m_Background);
    m_RotateBtn = new BmpButton(m_Parent);
    g_Widget->geometryFit(0, 14, 133, 80, m_RotateBtn);
    m_RotateBtn->setNormalBmpPath(QString(":/Images/ImageRotateNormal.png"));
    m_RotateBtn->setPressBmpPath(QString(":/Images/ImageRotatePress.png"));
    m_RotateBtn->setVisible(true);
    m_PreviousBtn = new BmpButton(m_Parent);
    g_Widget->geometryFit(133, 14, 133, 80, m_PreviousBtn);
    m_PreviousBtn->setNormalBmpPath(QString(":/Images/ImagePreviousNormal.png"));
    m_PreviousBtn->setPressBmpPath(QString(":/Images/ImagePreviousPress.png"));
    m_PreviousBtn->setVisible(true);
    m_ToggleBtn = new BmpButton(m_Parent);
    m_ToggleBtn->setNormalBmpPath(QString(":/Images/ImagePlayNormal.png"));
    m_ToggleBtn->setPressBmpPath(QString(":/Images/ImagePlayPress.png"));
    g_Widget->geometryFit(266, 14, 133, 80, m_ToggleBtn);
    m_ToggleBtn->setVisible(true);
    m_NextBtn = new BmpButton(m_Parent);
    g_Widget->geometryFit(399, 14, 133, 80, m_NextBtn);
    m_NextBtn->setNormalBmpPath(QString(":/Images/ImageNextNormal.png"));
    m_NextBtn->setPressBmpPath(QString(":/Images/ImageNextPress.png"));
    m_NextBtn->setVisible(true);
    m_ZoomOutBtn = new BmpButton(m_Parent);
    g_Widget->geometryFit(532, 14, 133, 80, m_ZoomOutBtn);
    m_ZoomOutBtn->setNormalBmpPath(QString(":/Images/ImageZoomOutNormal.png"));
    m_ZoomOutBtn->setPressBmpPath(QString(":/Images/ImageZoomOutPress.png"));
    m_ZoomOutBtn->setVisible(true);
    m_ZoomInBtn = new BmpButton(m_Parent);
    g_Widget->geometryFit(665, 14, 135, 80, m_ZoomInBtn);
    m_ZoomInBtn->setNormalBmpPath(QString(":/Images/ImageZoomInNormal.png"));
    m_ZoomInBtn->setPressBmpPath(QString(":/Images/ImageZoomInPress.png"));
    m_ZoomInBtn->setVisible(true);
    m_Parent->setVisible(true);
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
