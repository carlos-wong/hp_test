#include "SDDiskListViewWidget.h"
#include "SDDiskMusicListViewWidget/SDDiskMusicListViewWidget.h"
#include "SDDiskImageListViewWidget/SDDiskImageListViewWidget.h"
#include "SDDiskVideoListViewWidget/SDDiskVideoListViewWidget.h"
#include "BusinessLogic/BusinessLogicUtility.h"
#include "AutoConnect.h"
#include "BmpWidget.h"
#include <QPainter>

class SDDiskListViewWidgetPrivate
{
    Q_DISABLE_COPY(SDDiskListViewWidgetPrivate)
public:
    explicit SDDiskListViewWidgetPrivate(SDDiskListViewWidget* parent);
    ~SDDiskListViewWidgetPrivate();
    void initialize();
    void connectAllSlots();
private:
    SDDiskListViewWidget* m_Parent;
    BmpWidget* m_Background;
    SDDiskMusicListViewWidget* m_SDDiskMusicListViewWidget;
    SDDiskImageListViewWidget* m_SDDiskImageListViewWidget;
    SDDiskVideoListViewWidget* m_SDDiskVideoListViewWidget;
};

SDDiskListViewWidget::SDDiskListViewWidget(QWidget *parent)
    : QWidget(parent)
    , m_Private(new SDDiskListViewWidgetPrivate(this))
{
    setVisible(false);
}

SDDiskListViewWidget::~SDDiskListViewWidget()
{
}

void SDDiskListViewWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(196, 0, 604, 428, this);
}

void SDDiskListViewWidget::onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status)
{
    switch (destinationType) {
    case Widget::T_SDDiskMusic:
    case Widget::T_SDDiskImage:
    case Widget::T_SDDiskVideo: {
        if (WidgetStatus::Show == status) {
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

SDDiskListViewWidgetPrivate::SDDiskListViewWidgetPrivate(SDDiskListViewWidget *parent)
    : m_Parent(parent)
{
    m_Background = NULL;
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
    m_Background = new BmpWidget(m_Parent);
    m_Background->setBackgroundBmpPath(QString(":/Images/MultimediaListViewBackground.png"));
    g_Widget->geometryFit(0, 18, 586, 366, m_Background);
    m_Background->setVisible(true);
    m_SDDiskMusicListViewWidget = new SDDiskMusicListViewWidget(m_Parent);
    m_SDDiskImageListViewWidget = new SDDiskImageListViewWidget(m_Parent);
    m_SDDiskVideoListViewWidget = new SDDiskVideoListViewWidget(m_Parent);
}

void SDDiskListViewWidgetPrivate::connectAllSlots()
{
    connectSignalAndSlotByNamesake(g_Widget, m_Parent, ARKRECEIVER(onWidgetTypeChange(const Widget::Type, const Widget::Type, const QString &)));
}
