#include "SettingEqualizerWidget.h"
#include "AutoConnect.h"
#include "BusinessLogic/Widget.h"
#include "CustomItemDelegate.h"
#include "CustomListView.h"
#include "../../SettingGeneralSlider.h"
#include "BmpButton.h"
#include "TextWidget.h"
#include <QStandardItemModel>
#include <QPaintEvent>
#include <QPainter>

namespace SourceString {
static const QString Bass(QObject::tr("Bass"));
static const QString Middle(QObject::tr("Middle"));
static const QString Treble(QObject::tr("Treble"));
static const QString Standard(QObject::tr("Standard"));
static const QString Popular(QObject::tr("Popular"));
static const QString Classic(QObject::tr("Classic"));
static const QString Jazz(QObject::tr("Jazz"));
static const QString Custom(QObject::tr("Custom"));
}

class SettingEqualizerWidgetPrivate
{
public:
    explicit SettingEqualizerWidgetPrivate(SettingEqualizerWidget* parent);
    ~SettingEqualizerWidgetPrivate();
    void initializeParent();
    void initializeBasicWidget();
    void initializeTimer();
    void connectAllSlots();
private:
    Q_DECLARE_PUBLIC(SettingEqualizerWidget)
    SettingEqualizerWidget* const q_ptr;
    QStandardItemModel* m_StandardItemModel;
    CustomItemDelegate* m_CustomItemDelegate;
    CustomListView* m_CustomListView;
    SettingGeneralSlider* m_BassSlider;
    SettingGeneralSlider* m_MiddleSlider;
    SettingGeneralSlider* m_TrebleSlider;
    BmpButton* m_StandardBtn;
    BmpButton* m_PopularBtn;
    BmpButton* m_ClassicBtn;
    BmpButton* m_JazzBtn;
    BmpButton* m_CustomBtn;
    QTimer* m_Timer;
};

SettingEqualizerWidget::SettingEqualizerWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new SettingEqualizerWidgetPrivate(this))
{
}

SettingEqualizerWidget::~SettingEqualizerWidget()
{
    if (NULL != d_ptr) {
        delete d_ptr;
    }
}

void SettingEqualizerWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(180, 0, 600, 334, this);
}

void SettingEqualizerWidget::onEqualizerItemChange(const int item, const int bass, const int middle, const int treble)
{
    Q_D(SettingEqualizerWidget);
    if ((!d->m_BassSlider->isSliderDown())
            && (!d->m_MiddleSlider->isSliderDown())
            && (!d->m_TrebleSlider->isSliderDown())) {
        d->m_BassSlider->setValue(bass);
        d->m_MiddleSlider->setValue(middle);
        d->m_TrebleSlider->setValue(treble);
    }
    d->m_StandardBtn->setStatus(EI_Standard == item ? BmpButton::B_Press: BmpButton::B_Normal);
    d->m_PopularBtn->setStatus(EI_Popular == item ? BmpButton::B_Press: BmpButton::B_Normal);
    d->m_ClassicBtn->setStatus(EI_Classic == item ? BmpButton::B_Press: BmpButton::B_Normal);
    d->m_JazzBtn->setStatus(EI_Jazz == item ? BmpButton::B_Press: BmpButton::B_Normal);
    d->m_CustomBtn->setStatus(EI_Custom == item ? BmpButton::B_Press: BmpButton::B_Normal);
}

void SettingEqualizerWidget::onSliderMoved(const int value)
{
    Q_D(SettingEqualizerWidget);
    d->initializeTimer();
    d->m_Timer->start();
}

void SettingEqualizerWidget::onSliderReleased(const int value)
{
    Q_D(SettingEqualizerWidget);
    g_Audio->setEqualizerItem(EI_Custom, d->m_BassSlider->value(), d->m_MiddleSlider->value(), d->m_TrebleSlider->value());
}

void SettingEqualizerWidget::onToolButtonRelease()
{
    BmpButton* ptr = static_cast<BmpButton*>(sender());
    Q_D(SettingEqualizerWidget);
    if (ptr == d->m_StandardBtn) {
        g_Audio->setEqualizerItem(EI_Standard, EI_Undefine, EI_Undefine, EI_Undefine);
    } else if (ptr == d->m_PopularBtn) {
        g_Audio->setEqualizerItem(EI_Popular, EI_Undefine, EI_Undefine, EI_Undefine);
    } else if (ptr == d->m_ClassicBtn) {
        g_Audio->setEqualizerItem(EI_Classic, EI_Undefine, EI_Undefine, EI_Undefine);
    } else if (ptr == d->m_JazzBtn) {
        g_Audio->setEqualizerItem(EI_Jazz, EI_Undefine, EI_Undefine, EI_Undefine);
    } else if (ptr == d->m_CustomBtn) {
        g_Audio->setEqualizerItem(EI_Custom, EI_Undefine, EI_Undefine, EI_Undefine);
    }
}

void SettingEqualizerWidget::onTimeout()
{
    Q_D(SettingEqualizerWidget);
    g_Audio->setEqualizerItem(EI_Custom, d->m_BassSlider->value(), d->m_MiddleSlider->value(), d->m_TrebleSlider->value());
}

SettingEqualizerWidgetPrivate::SettingEqualizerWidgetPrivate(SettingEqualizerWidget *parent)
    : q_ptr(parent)
{
    m_StandardItemModel = NULL;
    m_CustomItemDelegate = NULL;
    m_CustomListView = NULL;
    m_BassSlider = NULL;
    m_MiddleSlider = NULL;
    m_TrebleSlider = NULL;
    m_StandardBtn = NULL;
    m_PopularBtn = NULL;
    m_ClassicBtn = NULL;
    m_JazzBtn = NULL;
    m_CustomBtn = NULL;
    m_Timer = NULL;
    initializeParent();
    initializeBasicWidget();
    connectAllSlots();
}

SettingEqualizerWidgetPrivate::~SettingEqualizerWidgetPrivate()
{
}

void SettingEqualizerWidgetPrivate::initializeParent()
{
    Q_Q(SettingEqualizerWidget);
    q->setVisible(true);
}

void SettingEqualizerWidgetPrivate::initializeBasicWidget()
{
    Q_Q(SettingEqualizerWidget);
    Qt::ConnectionType type = static_cast<Qt::ConnectionType>(Qt::AutoConnection | Qt::UniqueConnection);
    m_StandardBtn = new BmpButton(q);
    m_StandardBtn->setNormalBmpPath(QString(":/Images/SettingSoundEqualizerBlankNormal.png"));
    m_StandardBtn->setPressBmpPath(QString(":/Images/SettingSoundEqualizerBlankPress.png"));
    g_Widget->geometryFit(16, 240, 100, 54, m_StandardBtn);
    m_StandardBtn->setText(SourceString::Standard);
    m_StandardBtn->setLanguageType(BmpButton::T_Translate);
    m_StandardBtn->setVisible(true);
    QObject::connect(m_StandardBtn, ARKSENDER(released()),
                     q,             ARKRECEIVER(onToolButtonRelease()),
                     type);

    m_PopularBtn = new BmpButton(q);
    m_PopularBtn->setNormalBmpPath(QString(":/Images/SettingSoundEqualizerBlankNormal.png"));
    m_PopularBtn->setPressBmpPath(QString(":/Images/SettingSoundEqualizerBlankPress.png"));
    g_Widget->geometryFit(132, 240, 100, 54, m_PopularBtn);
    m_PopularBtn->setText(SourceString::Popular);
    m_PopularBtn->setLanguageType(BmpButton::T_Translate);
    m_PopularBtn->setVisible(true);
    QObject::connect(m_PopularBtn, ARKSENDER(released()),
                     q,            ARKRECEIVER(onToolButtonRelease()),
                     type);

    m_ClassicBtn = new BmpButton(q);
    m_ClassicBtn->setNormalBmpPath(QString(":/Images/SettingSoundEqualizerBlankNormal.png"));
    m_ClassicBtn->setPressBmpPath(QString(":/Images/SettingSoundEqualizerBlankPress.png"));
    g_Widget->geometryFit(248, 240, 100, 54, m_ClassicBtn);
    m_ClassicBtn->setText(SourceString::Classic);
    m_ClassicBtn->setLanguageType(BmpButton::T_Translate);
    m_ClassicBtn->setVisible(true);
    QObject::connect(m_ClassicBtn, ARKSENDER(released()),
                     q,            ARKRECEIVER(onToolButtonRelease()),
                     type);

    m_JazzBtn = new BmpButton(q);
    m_JazzBtn->setNormalBmpPath(QString(":/Images/SettingSoundEqualizerBlankNormal.png"));
    m_JazzBtn->setPressBmpPath(QString(":/Images/SettingSoundEqualizerBlankPress.png"));
    g_Widget->geometryFit(364, 240, 100, 54, m_JazzBtn);
    m_JazzBtn->setText(SourceString::Jazz);
    m_JazzBtn->setLanguageType(BmpButton::T_Translate);
    m_JazzBtn->setVisible(true);
    QObject::connect(m_JazzBtn, ARKSENDER(released()),
                     q,         ARKRECEIVER(onToolButtonRelease()),
                     type);

    m_CustomBtn = new BmpButton(q);
    m_CustomBtn->setNormalBmpPath(QString(":/Images/SettingSoundEqualizerBlankNormal.png"));
    m_CustomBtn->setPressBmpPath(QString(":/Images/SettingSoundEqualizerBlankPress.png"));
    g_Widget->geometryFit(480, 240, 100, 54, m_CustomBtn);
    m_CustomBtn->setText(SourceString::Custom);
    m_CustomBtn->setLanguageType(BmpButton::T_Translate);
    m_CustomBtn->setVisible(true);
    QObject::connect(m_CustomBtn, ARKSENDER(released()),
                     q,           ARKRECEIVER(onToolButtonRelease()),
                     type);
    EqualizerItem item = AudioPersistent::getEqualizerItem();
    m_StandardBtn->setStatus(EI_Standard == item ? BmpButton::B_Press: BmpButton::B_Normal);
    m_PopularBtn->setStatus(EI_Popular == item ? BmpButton::B_Press: BmpButton::B_Normal);
    m_ClassicBtn->setStatus(EI_Classic == item ? BmpButton::B_Press: BmpButton::B_Normal);
    m_JazzBtn->setStatus(EI_Jazz == item ? BmpButton::B_Press: BmpButton::B_Normal);
    m_CustomBtn->setStatus(EI_Custom == item ? BmpButton::B_Press: BmpButton::B_Normal);
    int bass;
    int middle;
    int treble;
    switch (item) {
    case EI_Standard: {
        bass = 7;
        middle = 7;
        treble = 7;
        break;
    }
    case EI_Popular: {
        bass = 7;
        middle = 7;
        treble = 11;
        break;
    }
    case EI_Classic: {
        bass = 11;
        middle = 9;
        treble = 2;
        break;
    }
    case EI_Jazz: {
        bass = 10;
        middle = 11;
        treble = 11;
        break;
    }
    default: {
        bass = AudioPersistent::getBassEqualizer();
        middle = AudioPersistent::getMiddleEqualizer();
        treble = AudioPersistent::getTrebleEqualizer();
        break;
    }
    }
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
    m_CustomListView->setIndexWidget(m_StandardItemModel->index(0, 0, QModelIndex()), m_BassSlider = new SettingGeneralSlider(m_CustomListView));
    m_BassSlider->setRange(0, 14);
    m_BassSlider->setValue(bass);
    m_BassSlider->setText(SourceString::Bass);
    QObject::connect(m_BassSlider, ARKSENDER(sliderMoved(const int)),
                     q,            ARKRECEIVER(onSliderMoved(const int)),
                     type);
    QObject::connect(m_BassSlider, ARKSENDER(sliderReleased(const int)),
                     q,            ARKRECEIVER(onSliderReleased(const int)),
                     type);
    m_CustomListView->setIndexWidget(m_StandardItemModel->index(1, 0, QModelIndex()), m_MiddleSlider = new SettingGeneralSlider(m_CustomListView));
    m_MiddleSlider->setRange(0, 14);
    m_MiddleSlider->setValue(middle);
    m_MiddleSlider->setText(SourceString::Middle);
    QObject::connect(m_MiddleSlider, ARKSENDER(sliderMoved(const int)),
                     q,              ARKRECEIVER(onSliderMoved(const int)),
                     type);
    QObject::connect(m_MiddleSlider, ARKSENDER(sliderReleased(const int)),
                     q,           ARKRECEIVER(onSliderReleased(const int)),
                     type);
    m_CustomListView->setIndexWidget(m_StandardItemModel->index(2, 0, QModelIndex()), m_TrebleSlider = new SettingGeneralSlider(m_CustomListView));
    m_TrebleSlider->setRange(0, 14);
    m_TrebleSlider->setValue(treble);
    m_TrebleSlider->setText(SourceString::Treble);
    QObject::connect(m_TrebleSlider, ARKSENDER(sliderMoved(const int)),
                     q,              ARKRECEIVER(onSliderMoved(const int)),
                     type);
    QObject::connect(m_TrebleSlider, ARKSENDER(sliderReleased(const int)),
                     q,            ARKRECEIVER(onSliderReleased(const int)),
                     type);
    g_Widget->geometryFit(0, 0, 600, 180, m_CustomListView);
}

void SettingEqualizerWidgetPrivate::initializeTimer()
{
    Q_Q(SettingEqualizerWidget);
    if (NULL == m_Timer) {
        m_Timer = new QTimer(q);
        m_Timer->setInterval(1);
        m_Timer->setSingleShot(true);
        QObject::connect(m_Timer, ARKSENDER(timeout()),
                         q,       ARKRECEIVER(onTimeout()));
    }
}

void SettingEqualizerWidgetPrivate::connectAllSlots()
{
    Q_Q(SettingEqualizerWidget);
    connectSignalAndSlotByNamesake(g_Audio, q, ARKRECEIVER(onEqualizerItemChange(const int, const int, const int, const int)));
}
