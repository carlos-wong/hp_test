#include "USBDiskListViewWidget.h"
#include "USBDiskMusicListViewWidget/USBDiskMusicListViewWidget.h"
#include "USBDiskImageListViewWidget/USBDiskImageListViewWidget.h"
#include "USBDiskVideoListViewWidget/USBDiskVideoListViewWidget.h"
#include "BusinessLogic/BusinessLogicUtility.h"
#include "AutoConnect.h"
#include "BmpWidget.h"
#include <QPainter>

class USBDiskListViewWidgetPrivate
{
    Q_DISABLE_COPY(USBDiskListViewWidgetPrivate)
public:
    explicit USBDiskListViewWidgetPrivate(USBDiskListViewWidget* parent);
    ~USBDiskListViewWidgetPrivate();
    void initialize();
    void connectAllSlots();
    BmpWidget* m_Background;
    USBDiskMusicListViewWidget* m_USBDiskMusicListViewWidget;
    USBDiskImageListViewWidget* m_USBDiskImageListViewWidget;
    USBDiskVideoListViewWidget* m_USBDiskVideoListViewWidget;
private:
    USBDiskListViewWidget* m_Parent;
};

USBDiskListViewWidget::USBDiskListViewWidget(QWidget *parent)
    : QWidget(parent)
    , m_Private(new USBDiskListViewWidgetPrivate(this))
{
    setVisible(false);
}

USBDiskListViewWidget::~USBDiskListViewWidget()
{
}

void USBDiskListViewWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(196, 0, 604, 428, this);
}

void USBDiskListViewWidget::onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status)
{
    switch (destinationType) {
    case Widget::T_USBDiskMusic:
    case Widget::T_USBDiskImage:
    case Widget::T_USBDiskVideo: {
        if (WidgetStatus::Show == status) {
            if (!isVisible()) {
                setVisible(true);
            }
        }
        break;
    }
    default: {
        if (WidgetStatus::Show == status) {
            if (isVisible()) {
                setVisible(false);
            }
        }
        break;
    }
    }
}

USBDiskListViewWidgetPrivate::USBDiskListViewWidgetPrivate(USBDiskListViewWidget *parent)
    : m_Parent(parent)
{
    m_Background = NULL;
    m_USBDiskMusicListViewWidget = NULL;
    m_USBDiskImageListViewWidget = NULL;
    m_USBDiskVideoListViewWidget = NULL;
    initialize();
    connectAllSlots();
}

USBDiskListViewWidgetPrivate::~USBDiskListViewWidgetPrivate()
{
}

void USBDiskListViewWidgetPrivate::initialize()
{
    m_Background = new BmpWidget(m_Parent);
    m_Background->setBackgroundBmpPath(QString(":/Images/MultimediaListViewBackground.png"));
    g_Widget->geometryFit(0, 18, 586, 366, m_Background);
    m_Background->setVisible(true);
    m_USBDiskMusicListViewWidget = new USBDiskMusicListViewWidget(m_Parent);
    m_USBDiskImageListViewWidget = new USBDiskImageListViewWidget(m_Parent);
    m_USBDiskVideoListViewWidget = new USBDiskVideoListViewWidget(m_Parent);
}

void USBDiskListViewWidgetPrivate::connectAllSlots()
{
    connectSignalAndSlotByNamesake(g_Widget, m_Parent, ARKRECEIVER(onWidgetTypeChange(const Widget::Type, const Widget::Type, const QString &)));
}
