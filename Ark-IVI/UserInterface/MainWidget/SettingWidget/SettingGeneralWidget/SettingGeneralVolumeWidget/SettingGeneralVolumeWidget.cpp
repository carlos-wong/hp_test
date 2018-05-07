#include "SettingGeneralVolumeWidget.h"
#include "AutoConnect.h"
#include "CustomListView.h"
#include "CustomItemDelegate.h"
#include "../../SettingGeneralSlider.h"
#include "BusinessLogic/Widget.h"
#include "BusinessLogic/Audio.h"
#include <QPainter>
#include <QStandardItemModel>
#include <QStandardItem>

namespace SourceString {
static const QString Volume(QObject::tr("Volume"));
}

class SettingGeneralVolumeWidgetPrivate
{
public:
    explicit SettingGeneralVolumeWidgetPrivate(SettingGeneralVolumeWidget* parent);
    ~SettingGeneralVolumeWidgetPrivate();
    void initializeParent();
    void initializeBasicWidget();
    void initializeTimer();
    void connectAllSlots();
private:
    Q_DECLARE_PUBLIC(SettingGeneralVolumeWidget)
    SettingGeneralVolumeWidget* const q_ptr;
    QStandardItemModel* m_QStandardItemModel;
    CustomItemDelegate* m_CustomItemDelegate;
    CustomListView* m_CustomListView;
    SettingGeneralSlider* m_Volume;
    QTimer* m_Timer;
};

SettingGeneralVolumeWidget::SettingGeneralVolumeWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new SettingGeneralVolumeWidgetPrivate(this))
{
}

SettingGeneralVolumeWidget::~SettingGeneralVolumeWidget()
{
    if (NULL != d_ptr) {
        delete d_ptr;
    }
}
void SettingGeneralVolumeWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(180, 0, 600, 334, this);
}

void SettingGeneralVolumeWidget::onVolumeChange(const int volume)
{
    Q_D(SettingGeneralVolumeWidget);
    d->m_Volume->setValue(volume);
}

void SettingGeneralVolumeWidget::onTimeout()
{
    Q_D(SettingGeneralVolumeWidget);
    g_Audio->requestSetVolume(d->m_Volume->value());
}

void SettingGeneralVolumeWidget::onSliderMoved(const int value)
{
    onVolumeChange(value);
    Q_D(SettingGeneralVolumeWidget);
    d->initializeTimer();
    if (d->m_Timer->isActive()) {
        d->m_Timer->stop();
    }
    d->m_Timer->start();
}

void SettingGeneralVolumeWidget::onSliderReleased(const int value)
{
    g_Audio->requestSetVolume(value);
}

SettingGeneralVolumeWidgetPrivate::SettingGeneralVolumeWidgetPrivate(SettingGeneralVolumeWidget *parent)
    : q_ptr(parent)
{
    m_QStandardItemModel = NULL;
    m_CustomItemDelegate = NULL;
    m_CustomListView = NULL;
    m_Volume = NULL;
    m_Timer = NULL;
    initializeParent();
    initializeBasicWidget();
    connectAllSlots();
}

SettingGeneralVolumeWidgetPrivate::~SettingGeneralVolumeWidgetPrivate()
{
}

void SettingGeneralVolumeWidgetPrivate::initializeParent()
{
    Q_Q(SettingGeneralVolumeWidget);
    q->setVisible(false);
}

void SettingGeneralVolumeWidgetPrivate::initializeBasicWidget()
{
    Q_Q(SettingGeneralVolumeWidget);
    m_QStandardItemModel = new QStandardItemModel(q);
    QStandardItem* rootItem = m_QStandardItemModel->invisibleRootItem();
    QStandardItem* item = new QStandardItem();
    item->setSizeHint(QSize(600 * g_Widget->widthScalabilityFactor(), 60 * g_Widget->heightScalabilityFactor()));
    rootItem->setChild(0, 0, item);
    m_CustomItemDelegate = new CustomItemDelegate(q);
    m_CustomListView = new CustomListView(q);
    m_CustomListView->setItemDelegate(m_CustomItemDelegate);
    m_CustomListView->setModel(m_QStandardItemModel);
    m_CustomListView->setIndexWidget(m_QStandardItemModel->index(0, 0, QModelIndex()), m_Volume = new SettingGeneralSlider(m_CustomListView));
    m_Volume->setText(SourceString::Volume);
    m_Volume->setRange(0, 40);
    m_Volume->setValue(AudioPersistent::getVolume());
    g_Widget->geometryFit(0, 0, 600, 60, m_CustomListView);
    Qt::ConnectionType type = static_cast<Qt::ConnectionType>(Qt::AutoConnection | Qt::UniqueConnection);
    QObject::connect(m_Volume, ARKSENDER(sliderMoved(const int)),
                     q,        ARKRECEIVER(onSliderMoved(const int)),
                     type);
    QObject::connect(m_Volume, ARKSENDER(sliderReleased(const int)),
                     q,        ARKRECEIVER(onSliderReleased(const int)),
                     type);
}

void SettingGeneralVolumeWidgetPrivate::initializeTimer()
{
    Q_Q(SettingGeneralVolumeWidget);
    if (NULL == m_Timer) {
        m_Timer = new QTimer(q);
        m_Timer->setInterval(1);
        m_Timer->setSingleShot(true);
        QObject::connect(m_Timer, ARKSENDER(timeout()),
                         q,       ARKRECEIVER(onTimeout()));
    }
}

void SettingGeneralVolumeWidgetPrivate::connectAllSlots()
{
    Q_Q(SettingGeneralVolumeWidget);
    connectSignalAndSlotByNamesake(g_Audio, q, ARKRECEIVER(onVolumeChange(const int)));
}
