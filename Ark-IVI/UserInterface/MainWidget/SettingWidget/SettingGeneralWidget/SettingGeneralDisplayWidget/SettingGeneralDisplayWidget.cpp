#include "SettingGeneralDisplayWidget.h"
#include "AutoConnect.h"
#include "BmpWidget.h"
#include "BmpButton.h"
#include "CustomListView.h"
#include "CustomItemDelegate.h"
#include "../../SettingGeneralSlider.h"
#include "BusinessLogic/Widget.h"
#include "BusinessLogic/Setting.h"
#include <QPainter>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QDebug>

namespace SourceString {
static const QString Brightness(QObject::tr("Brightness"));
static const QString Constrast(QObject::tr("Constrast"));
static const QString Saturation(QObject::tr("Saturation"));
}

class SettingGeneralDisplayWidgetPrivate
{
public:
    explicit SettingGeneralDisplayWidgetPrivate(SettingGeneralDisplayWidget* parent);
    ~SettingGeneralDisplayWidgetPrivate();
    void initializeParent();
    void initializeBasicWidget();
    void connectAllSlots();
private:
    Q_DECLARE_PUBLIC(SettingGeneralDisplayWidget)
    SettingGeneralDisplayWidget* const q_ptr;
    QStandardItemModel* m_StandardItemModel;
    CustomItemDelegate* m_CustomItemDelegate;
    CustomListView* m_CustomListView;
    SettingGeneralSlider* m_Brightness;
    SettingGeneralSlider* m_Constrast;
    SettingGeneralSlider* m_Saturation;
};

SettingGeneralDisplayWidget::SettingGeneralDisplayWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new SettingGeneralDisplayWidgetPrivate(this))
{
}

SettingGeneralDisplayWidget::~SettingGeneralDisplayWidget()
{
    if (NULL != d_ptr) {
        delete d_ptr;
    }
}

void SettingGeneralDisplayWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(180, 0, 600, 334, this);
}

void SettingGeneralDisplayWidget::onBrightnessChange(const int value)
{
    Q_D(SettingGeneralDisplayWidget);
    d->m_Brightness->setValue(value);
}

void SettingGeneralDisplayWidget::onContrastChange(const int value)
{
    Q_D(SettingGeneralDisplayWidget);
    d->m_Constrast->setValue(value);
}

void SettingGeneralDisplayWidget::onSaturationChange(const int value)
{
    Q_D(SettingGeneralDisplayWidget);
    d->m_Saturation->setValue(value);
}

void SettingGeneralDisplayWidget::onSliderReleased(const int value)
{
    SettingGeneralSlider* ptr = static_cast<SettingGeneralSlider*>(sender());
    Q_D(SettingGeneralDisplayWidget);
    if (ptr == d->m_Brightness) {
        g_Setting->setBrightness(ST_Value, value);
    } else if (ptr == d->m_Constrast) {
        g_Setting->setContrast(ST_Value, value);
    } else if (ptr == d->m_Saturation) {
        g_Setting->setSaturation(ST_Value, value);
    }
}

SettingGeneralDisplayWidgetPrivate::SettingGeneralDisplayWidgetPrivate(SettingGeneralDisplayWidget *parent)
    : q_ptr(parent)
{
    m_StandardItemModel = NULL;
    m_CustomItemDelegate = NULL;
    m_CustomListView = NULL;
    m_Brightness = NULL;
    m_Constrast = NULL;
    m_Saturation = NULL;
    initializeParent();
    initializeBasicWidget();
    connectAllSlots();
}

SettingGeneralDisplayWidgetPrivate::~SettingGeneralDisplayWidgetPrivate()
{
}

void SettingGeneralDisplayWidgetPrivate::initializeParent()
{
    Q_Q(SettingGeneralDisplayWidget);
    q->setVisible(false);
}

void SettingGeneralDisplayWidgetPrivate::initializeBasicWidget()
{
    Q_Q(SettingGeneralDisplayWidget);
    m_StandardItemModel = new QStandardItemModel(q);
    QStandardItem* rootItem = m_StandardItemModel->invisibleRootItem();
    for (int i = 0; i < 3; ++i) {
        QStandardItem* item = new QStandardItem();
        item->setSizeHint(QSize(600 * g_Widget->widthScalabilityFactor(), 60 * g_Widget->heightScalabilityFactor()));
        rootItem->setChild(i, 0, item);
    }
    m_CustomItemDelegate = new CustomItemDelegate(q);
    m_CustomListView = new CustomListView(q);
    m_CustomListView->setItemDelegate(m_CustomItemDelegate);
    m_CustomListView->setModel(m_StandardItemModel);
    m_CustomListView->setIndexWidget(m_StandardItemModel->index(0, 0, QModelIndex()), m_Brightness = new SettingGeneralSlider(m_CustomListView));
    m_Brightness->setRange(0, 14);
    m_Brightness->setValue(SettingPersistent::getBrightness());
    m_Brightness->setText(SourceString::Brightness);
    Qt::ConnectionType type = static_cast<Qt::ConnectionType>(Qt::AutoConnection | Qt::UniqueConnection);
    QObject::connect(m_Brightness, ARKSENDER(sliderMoved(const int)),
                     q,            ARKRECEIVER(onSliderReleased(const int)),
                     type);
    QObject::connect(m_Brightness, ARKSENDER(sliderReleased(const int)),
                     q,            ARKRECEIVER(onSliderReleased(const int)),
                     type);
    m_CustomListView->setIndexWidget(m_StandardItemModel->index(1, 0, QModelIndex()), m_Constrast = new SettingGeneralSlider(m_CustomListView));
    m_Constrast->setRange(0, 14);
    m_Constrast->setValue(SettingPersistent::getContrast());
    m_Constrast->setText(SourceString::Constrast);
    QObject::connect(m_Constrast, ARKSENDER(sliderMoved(const int)),
                     q,           ARKRECEIVER(onSliderReleased(const int)),
                     type);
    QObject::connect(m_Constrast, ARKSENDER(sliderReleased(const int)),
                     q,           ARKRECEIVER(onSliderReleased(const int)),
                     type);
    m_CustomListView->setIndexWidget(m_StandardItemModel->index(2, 0, QModelIndex()), m_Saturation = new SettingGeneralSlider(m_CustomListView));
    m_Saturation->setRange(0, 14);
    m_Saturation->setValue(SettingPersistent::getSaturation());
    m_Saturation->setText(SourceString::Saturation);
    QObject::connect(m_Saturation, ARKSENDER(sliderMoved(const int)),
                     q,            ARKRECEIVER(onSliderReleased(const int)),
                     type);
    QObject::connect(m_Saturation, ARKSENDER(sliderReleased(const int)),
                     q,            ARKRECEIVER(onSliderReleased(const int)),
                     type);
    g_Widget->geometryFit(0, 0, 600, 180, m_CustomListView);
}

void SettingGeneralDisplayWidgetPrivate::connectAllSlots()
{
    Q_Q(SettingGeneralDisplayWidget);
    connectSignalAndSlotByNamesake(g_Setting, q, ARKRECEIVER(onBrightnessChange(const int)));
    connectSignalAndSlotByNamesake(g_Setting, q, ARKRECEIVER(onContrastChange(const int)));
    connectSignalAndSlotByNamesake(g_Setting, q, ARKRECEIVER(onSaturationChange(const int)));
}
