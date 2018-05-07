#include "SDDiskListViewWidget.h"
#include "SDDiskMusicListViewWidget/SDDiskMusicListViewWidget.h"
#include "SDDiskImageListViewWidget/SDDiskImageListViewWidget.h"
#include "SDDiskVideoListViewWidget/SDDiskVideoListViewWidget.h"
#include "BusinessLogicUtility.h"
#include "AutoConnect.h"

class SDDiskListViewWidgetPrivate
{
    Q_DISABLE_COPY(SDDiskListViewWidgetPrivate)
public:
    explicit SDDiskListViewWidgetPrivate(SDDiskListViewWidget* parent);
    ~SDDiskListViewWidgetPrivate();
    void initialize();
    void connectAllSlots();
    SDDiskMusicListViewWidget* m_SDDiskMusicListViewWidget;
    SDDiskImageListViewWidget* m_SDDiskImageListViewWidget;
    SDDiskVideoListViewWidget* m_SDDiskVideoListViewWidget;
private:
    SDDiskListViewWidget* m_Parent;
};

SDDiskListViewWidget::SDDiskListViewWidget(QWidget *parent)
    : QWidget(parent)
    , Widget::Interface()
    , Multimedia::Interface()
    , m_Private(new SDDiskListViewWidgetPrivate(this))
{
}

SDDiskListViewWidget::~SDDiskListViewWidget()
{
}

void SDDiskListViewWidget::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
}

void SDDiskListViewWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(278, 0, g_Widget->baseWindowWidth() - 278, g_Widget->baseWindowHeight(), this);
    QWidget::resizeEvent(event);
}

void SDDiskListViewWidget::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);
}

void SDDiskListViewWidget::ontWidgetTypeChange(const Widget::Type type, const QString &status)
{
    if (WidgetStatus::Show == status) {
        switch (type) {
        case Widget::T_SDDisk:
        case Widget::T_SDDiskMusic:
        case Widget::T_SDDiskImage:
        case Widget::T_SDDiskVideo: {
            setVisible(true);
            break;
        }
        case Widget::T_Music:
        case Widget::T_Image:
        case Widget::T_Video: {
            setVisible(false);
            break;
        }
        default: {
            break;
        }
        }
    }
}

SDDiskListViewWidgetPrivate::SDDiskListViewWidgetPrivate(SDDiskListViewWidget *parent)
    : m_Parent(parent)
{
    m_SDDiskMusicListViewWidget = NULL;
    m_SDDiskImageListViewWidget = NULL;
    m_SDDiskVideoListViewWidget = NULL;
    initialize();
    connectAllSlots();
}

SDDiskListViewWidgetPrivate::~SDDiskListViewWidgetPrivate()
{
}

void SDDiskListViewWidgetPrivate::initialize()
{
    if (NULL == m_SDDiskMusicListViewWidget) {
        m_SDDiskMusicListViewWidget = new SDDiskMusicListViewWidget(m_Parent);
    }
    if (NULL == m_SDDiskImageListViewWidget) {
        m_SDDiskImageListViewWidget = new SDDiskImageListViewWidget(m_Parent);
    }
    if (NULL == m_SDDiskVideoListViewWidget) {
        m_SDDiskVideoListViewWidget = new SDDiskVideoListViewWidget(m_Parent);
    }
}

void SDDiskListViewWidgetPrivate::connectAllSlots()
{
    connectSignalAndSlotByNamesake(g_Widget, m_Parent);
    connectSignalAndSlotByNamesake(g_Multimedia, m_Parent);
}
