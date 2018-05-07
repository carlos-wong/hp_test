#include "RadioToolWidget.h"
#include "AutoConnect.h"
#include "BmpButton.h"
#include "BmpWidget.h"
#include "BusinessLogic/Widget.h"
#include <QDebug>

class RadioToolWidgetPrivate {
public:
    explicit RadioToolWidgetPrivate(RadioToolWidget* parent);
    ~RadioToolWidgetPrivate();
    void initializeParent();
    void initializeBasicWidget();
private:
    Q_DECLARE_PUBLIC(RadioToolWidget)
    RadioToolWidget* const q_ptr;
    BmpWidget* m_Background;
    BmpButton* m_BandBtn;
    BmpButton* m_StereoBtn;
    BmpButton* m_LocBtn;
    BmpButton* m_SearchBtn;
    BmpButton* m_ScanBtn;
};

RadioToolWidget::RadioToolWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new RadioToolWidgetPrivate(this))
{
}

RadioToolWidget::~RadioToolWidget()
{
    if (NULL != d_ptr) {
        delete d_ptr;
    }
}

void RadioToolWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(0, 387, 800, 94, this);
}

void RadioToolWidget::onToolButtonRelease()
{
    Q_D(RadioToolWidget);
    BmpButton* ptr = static_cast<BmpButton*>(sender());
    if (ptr == d->m_BandBtn) {
        g_Radio->toggleBand();
    } else if (ptr == d->m_StereoBtn) {
        g_Radio->toggleStereo();
    } else if (ptr == d->m_LocBtn) {
        g_Radio->toggleLoc();
    } else if (ptr == d->m_SearchBtn) {
        g_Radio->setCurrentBandSearch();
    }else if (ptr == d->m_ScanBtn) {
        g_Radio->setCurrentBandBrowse();
    }
}

void RadioToolWidget::onRadioInfoChange(const RadioBandType type, const unsigned short frequency,
                                               const unsigned short index, const unsigned short min, const unsigned short max, const unsigned short step)
{
    Q_UNUSED(frequency);
    Q_UNUSED(index);
    Q_UNUSED(min);
    Q_UNUSED(max);
    Q_UNUSED(step);

    Q_D(RadioToolWidget);
    switch (type) {
    case RBT_AM1:
    case RBT_AM2:
        if  (NULL != d->m_StereoBtn) {
            if (d->m_StereoBtn->isEnabled()) {
                d->m_StereoBtn->setEnabled(false);
                d->m_StereoBtn->setNormalBmpPath(QString(":/Images/hw_01_radio_stereo_disabled.png"));
            }
        }
        if  (NULL != d->m_LocBtn) {
            if (d->m_LocBtn->isEnabled()) {
                d->m_LocBtn->setEnabled(false);
                d->m_LocBtn->setNormalBmpPath(QString(":/Images/hw_01_radio_loc_disabled.png"));
            }
        }
        break;
    default:
        if  (NULL != d->m_StereoBtn) {
            if (!d->m_StereoBtn->isEnabled()) {
                d->m_StereoBtn->setEnabled(true);
                d->m_StereoBtn->setNormalBmpPath(QString(":/Images/hw_01_radio_stereo_normal.png"));
            }
        }
        if  (NULL != d->m_LocBtn) {
            if (!d->m_LocBtn->isEnabled()) {
                d->m_LocBtn->setEnabled(true);
                d->m_LocBtn->setNormalBmpPath(QString(":/Images/hw_01_radio_loc_normal.png"));
            }
        }
        break;
    }
}

RadioToolWidgetPrivate::RadioToolWidgetPrivate(RadioToolWidget *parent)
    : q_ptr(parent)
{
    m_Background = NULL;
    m_BandBtn = NULL;
    m_StereoBtn = NULL;
    m_LocBtn = NULL;
    m_SearchBtn = NULL;
    m_ScanBtn = NULL;
    initializeParent();
    initializeBasicWidget();
}

RadioToolWidgetPrivate::~RadioToolWidgetPrivate()
{
}

void RadioToolWidgetPrivate::initializeParent()
{
    Q_Q(RadioToolWidget);
    q->setVisible(true);
}

void RadioToolWidgetPrivate::initializeBasicWidget()
{
    Q_Q(RadioToolWidget);

    int btnY = 7;
    int btnWidth = 160;
    int btnHeight = 94;

    m_Background = new BmpWidget(q);
    m_Background->setBackgroundBmpPath(QString(":/Images/hw_01_radio_bottom_toolBar.png"));
    g_Widget->geometryFit(0, btnY, 800, btnHeight, m_Background);
    m_Background->setVisible(true);

    Qt::ConnectionType type = static_cast<Qt::ConnectionType>(Qt::UniqueConnection | Qt::AutoConnection);
    m_BandBtn = new BmpButton(q);
    m_BandBtn->setNormalBmpPath(QString(":/Images/hw_01_radio_band_normal.png"));
    m_BandBtn->setPressBmpPath(QString(":/Images/hw_01_radio_band_pressed.png"));
    g_Widget->geometryFit(0, btnY, btnWidth-1, btnHeight, m_BandBtn);
    m_BandBtn->setVisible(true);
    QObject::connect(m_BandBtn, ARKSENDER(released()),
                     q,         ARKRECEIVER(onToolButtonRelease()),
                     type);

    m_StereoBtn = new BmpButton(q);
    m_StereoBtn->setNormalBmpPath(QString(":/Images/hw_01_radio_stereo_normal.png"));
    m_StereoBtn->setPressBmpPath(QString(":/Images/hw_01_radio_stereo_pressed.png"));
    g_Widget->geometryFit(btnWidth+1, btnY, btnWidth-2, btnHeight, m_StereoBtn);
    m_StereoBtn->setVisible(true);
    QObject::connect(m_StereoBtn, ARKSENDER(released()),
                     q,           ARKRECEIVER(onToolButtonRelease()),
                     type);

    m_LocBtn = new BmpButton(q);
    m_LocBtn->setNormalBmpPath(QString(":/Images/hw_01_radio_loc_normal.png"));
    m_LocBtn->setPressBmpPath(QString(":/Images/hw_01_radio_loc_pressed.png"));
    g_Widget->geometryFit(btnWidth*3+2, btnY, btnWidth-2, btnHeight, m_LocBtn);
    m_LocBtn->setVisible(true);
    QObject::connect(m_LocBtn, ARKSENDER(released()),
                     q,        ARKRECEIVER(onToolButtonRelease()),
                     type);

    m_SearchBtn = new BmpButton(q);
    m_SearchBtn->setNormalBmpPath(QString(":/Images/hw_01_radio_search_normal.png"));
    m_SearchBtn->setPressBmpPath(QString(":/Images/hw_01_radio_search_pressed.png"));
    g_Widget->geometryFit(btnWidth*2+2, btnY, btnWidth-3, btnHeight, m_SearchBtn);
    m_SearchBtn->setVisible(true);
    QObject::connect(m_SearchBtn, ARKSENDER(released()),
                     q,           ARKRECEIVER(onToolButtonRelease()),
                     type);

    m_ScanBtn = new BmpButton(q);
    m_ScanBtn->setNormalBmpPath(QString(":/Images/hw_01_radio_headset_normal.png"));
    m_ScanBtn->setPressBmpPath(QString(":/Images/hw_01_radio_headset_pressed.png"));
    g_Widget->geometryFit(btnWidth*4+2, btnY, btnWidth-2, btnHeight, m_ScanBtn);
    m_ScanBtn->setVisible(true);
    QObject::connect(m_ScanBtn, ARKSENDER(released()),
                     q,         ARKRECEIVER(onToolButtonRelease()),
                     type);

    connectSignalAndSlotByNamesake(g_Radio, q, ARKRECEIVER(onRadioInfoChange(const RadioBandType , const unsigned short ,
                                                                             const unsigned short , const unsigned short , const unsigned short , const unsigned short)));

}

