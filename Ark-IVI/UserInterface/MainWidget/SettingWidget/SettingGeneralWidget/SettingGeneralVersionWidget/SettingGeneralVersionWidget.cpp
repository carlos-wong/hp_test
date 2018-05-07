#include "SettingGeneralVersionWidget.h"
#include "AutoConnect.h"
#include "BmpWidget.h"
#include "BmpButton.h"
#include "CustomItemDelegate.h"
#include "CustomListView.h"
#include "Utility.h"
#include "SettingPersistent.h"
#include "BusinessLogic/Widget.h"
#include "BmpButton.h"
#include "../../SettingMessageDialog.h"
#include "BusinessLogic/Audio.h"
#include "BusinessLogic/Bluetooth.h"
#include "BusinessLogic/Link.h"
#include "BusinessLogic/Multimedia.h"
#include "BusinessLogic/Radio.h"
#include "BusinessLogic/Setting.h"
#include "BusinessLogic/Widget.h"
#include <QDebug>
#include <QStandardItemModel>
#include <QRegExp>
#include <QFile>
#include <QPainter>
#include <stdlib.h>

namespace SourceString {
static const QString Reset(QObject::tr("Reset"));
static const QString Update_MCU(QObject::tr("UpdateMCU"));
static const QString Reset_Tip(QObject::tr("Reset Device Will Delete History And Reboot!"));
static const QString Rebooting(QObject::tr("Rebooting..."));
static const QString Confirm(QObject::tr("Confirm"));
static const QString Cancel(QObject::tr("Cancel"));
//static const QString Can_Not_Found_Upgrade_File(QObject::tr("Can't Found Upgrade File mcu.bin"));
static const QString Upgrade_Tip(QObject::tr("Upgrade MCU Only Handle Update Message!"));
static const QString Upgrade_Please_Wait_A_Moment(QObject::tr("Upgrade, Please Wait A Moment"));
static const QString Upgrade_Success(QObject::tr("Upgrade Success!"));
}

class VersionVariant
{
public:
    VersionVariant() {}
    ~VersionVariant() {}
    QString m_Text;
};
Q_DECLARE_METATYPE(VersionVariant)

class VersionDelegate : public CustomItemDelegate
{
    Q_DISABLE_COPY(VersionDelegate)
public:
    explicit VersionDelegate(QObject* parent = NULL);
    ~VersionDelegate();
protected:
    void paint(QPainter* painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const;
private:
    QScopedPointer<QPixmap> m_Rules;
};

class SettingGeneralVersionWidgetPrivate
{
    Q_DISABLE_COPY(SettingGeneralVersionWidgetPrivate)
public:
    explicit SettingGeneralVersionWidgetPrivate(SettingGeneralVersionWidget* parent);
    ~SettingGeneralVersionWidgetPrivate();
    void initializeParent();
    void initializeBasicWidget();
    void connectAllSlots();
    void initializeResetMessageDialog();
    void initializeUpdateMCUMessageDialog();
private:
    Q_DECLARE_PUBLIC(SettingGeneralVersionWidget)
    SettingGeneralVersionWidget* q_ptr;
    QStandardItemModel* m_StandardItemModel;
    VersionDelegate* m_VersionDelegate;
    CustomListView* m_CustomListView;
    BmpButton* m_UpdateMCUBtn;
    BmpButton* m_ResetBtn;
    SettingMessageDialog* m_ResetMessageDialog;
    SettingMessageDialog* m_UpdateMCUMessageDialog;
};

SettingGeneralVersionWidget::SettingGeneralVersionWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new SettingGeneralVersionWidgetPrivate(this))
{
}

SettingGeneralVersionWidget::~SettingGeneralVersionWidget()
{
    if (NULL != d_ptr) {
        delete d_ptr;
    }
}

void SettingGeneralVersionWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(180, 0, 600, 334, this);
}

void SettingGeneralVersionWidget::onArkProtocol(const ArkProtocolWrapper &protocol)
{
    switch (protocol.type) {
    case AT_Car: {
        struct ArkCar* car = arkProtocolWrapperToArkCar(protocol);
        if (NULL != car) {
            switch (car->type) {
            case ACT_MCUUpdate: {
                if (ACS_MCUUpdatePercent == car->status) {
                    if (1 == car->length) {
                        Q_D(SettingGeneralVersionWidget);
                        d->m_UpdateMCUMessageDialog->setTitle(QObject::tr(SourceString::Upgrade_Please_Wait_A_Moment.toLocal8Bit().constData()) + QString("(") + QString::number(car->data[0]) + QString("%)"));
                    } else {
                        qDebug() << "ACS_MCUUpdatePercent length error!" << car->length;
                    }
                }
                break;
            }
            default: {
                break;
            }
            }
        }
        break;
    }
    default: {
        break;
    }
    }
}

void SettingGeneralVersionWidget::onDeviceWatcherStatus(const DeviceWatcherType type, const DeviceWatcherStatus status)
{
    qDebug() << __PRETTY_FUNCTION__ << __LINE__ << type << status;
    if (DWT_USBDisk == type) {
        switch (status) {
        case DWS_Ready: {
            Q_D(SettingGeneralVersionWidget);
            d->m_UpdateMCUBtn->setVisible(QFile::exists(QString("/media/udisk/mcu.bin")));
            break;
        }
        default: {
            Q_D(SettingGeneralVersionWidget);
            d->m_UpdateMCUBtn->setVisible(false);
            break;
        }
        }
    }
}

void SettingGeneralVersionWidget::onBmpButtonRelease()
{
    Q_D(SettingGeneralVersionWidget);
    BmpButton* ptr = static_cast<BmpButton*>(sender());
    if (ptr == d->m_UpdateMCUBtn) {
        if (QFile::exists(QString("/media/udisk/mcu.bin"))) {
            d->initializeUpdateMCUMessageDialog();
            d->m_UpdateMCUMessageDialog->setTitle(QObject::tr(SourceString::Upgrade_Tip.toLocal8Bit().constData()));
            d->m_UpdateMCUMessageDialog->setVisible(true);
        } else {
            d->m_UpdateMCUBtn->setVisible(false);
        }
    } else if (ptr == d->m_ResetBtn) {
        d->initializeResetMessageDialog();
        d->m_ResetMessageDialog->setTitle(SourceString::Reset_Tip);
        d->m_ResetMessageDialog->setVisible(true);
    }
}

void SettingGeneralVersionWidget::onConfirm()
{
    Q_D(SettingGeneralVersionWidget);
    SettingMessageDialog* ptr = static_cast<SettingMessageDialog*>(sender());
    if (ptr == d->m_ResetMessageDialog) {
        if (NULL != d->m_ResetMessageDialog) {
            d->m_ResetMessageDialog->setTitle(SourceString::Rebooting);
            d->m_ResetMessageDialog->setBtnEnable(false);
            d->m_ResetMessageDialog->setVisible(true);
            g_Audio->reset();
            g_Bluetooth->reset();
            g_Link->reset();
            g_Multimedia->reset();
            g_Radio->reset();
            g_Setting->reset();
            g_Widget->reset();
            system("sync");
            if (0 != system("reboot")) {
                qCritical() << __PRETTY_FUNCTION__ << "!!!!!!!!!!!factory poweroff faile!!!!!!!!!!!";
            }
        }
    } else if (ptr == d->m_UpdateMCUMessageDialog) {
        d->m_UpdateMCUMessageDialog->setTitle(QObject::tr(SourceString::Upgrade_Please_Wait_A_Moment.toLocal8Bit().constData()) + QString("(0%)"));
        d->m_UpdateMCUMessageDialog->setBtnVisible(false);
        d->m_UpdateMCUMessageDialog->setVisible(true);
        if (g_Setting->startMCUUpgrade()) {
            g_Widget->acquireWidgetType(Widget::T_SettingGeneral);
        } else {
            d->m_UpdateMCUMessageDialog->setVisible(false);
        }
    }
}

void SettingGeneralVersionWidget::onCancel()
{
    SettingMessageDialog* ptr = static_cast<SettingMessageDialog*>(sender());
    Q_D(SettingGeneralVersionWidget);
    if (ptr == d->m_ResetMessageDialog) {
        if (NULL != d->m_ResetMessageDialog) {
            d->m_ResetMessageDialog->setBtnEnable(true);
            d->m_ResetMessageDialog->setVisible(false);
        }
    } else if (ptr == d->m_UpdateMCUMessageDialog) {
        if (NULL != d->m_UpdateMCUMessageDialog) {
            d->m_UpdateMCUMessageDialog->setBtnEnable(true);
            d->m_UpdateMCUMessageDialog->setVisible(false);
        }
    }
}

SettingGeneralVersionWidgetPrivate::SettingGeneralVersionWidgetPrivate(SettingGeneralVersionWidget *parent)
    : q_ptr(parent)
{
    m_StandardItemModel = NULL;
    m_VersionDelegate = NULL;
    m_CustomListView = NULL;
    m_UpdateMCUBtn = NULL;
    m_ResetBtn = NULL;
    m_ResetMessageDialog = NULL;
    m_UpdateMCUMessageDialog = NULL;
    initializeParent();
    initializeBasicWidget();
    connectAllSlots();
}

SettingGeneralVersionWidgetPrivate::~SettingGeneralVersionWidgetPrivate()
{
}

void SettingGeneralVersionWidgetPrivate::initializeParent()
{
    Q_Q(SettingGeneralVersionWidget);
    q->setVisible(true);
}

void SettingGeneralVersionWidgetPrivate::initializeBasicWidget()
{
    Q_Q(SettingGeneralVersionWidget);
    m_StandardItemModel = new QStandardItemModel(q);
    m_VersionDelegate = new VersionDelegate(q);
    m_CustomListView = new CustomListView(q);
    m_CustomListView->setItemDelegate(m_VersionDelegate);
    QStandardItem* root = m_StandardItemModel->invisibleRootItem();

    QStandardItem* item = NULL;
    VersionVariant variant;

    item = new QStandardItem();
    item->setSizeHint(QSize(600 * g_Widget->widthScalabilityFactor(), 60 * g_Widget->heightScalabilityFactor()));
    //ARK196-20170706-2243
    variant.m_Text = QString("APP:ARK169-") + compilerDate() + QString("V1.0");
    item->setData(qVariantFromValue(variant), Qt::DisplayRole);
    root->setChild(root->rowCount(), 0, item);

    item = new QStandardItem();
    item->setSizeHint(QSize(600 * g_Widget->widthScalabilityFactor(), 60 * g_Widget->heightScalabilityFactor()));
    //ARK169-20170706-2243
    variant.m_Text = QString("BSP:ARK169-") + osVersion() + QString("V1.0");
    item->setData(qVariantFromValue(variant), Qt::DisplayRole);
    root->setChild(root->rowCount(), 0, item);

    item = new QStandardItem();
    item->setSizeHint(QSize(600 * g_Widget->widthScalabilityFactor(), 60 * g_Widget->heightScalabilityFactor()));
    //CW6633-20170706-2243
    variant.m_Text = QString("MCU:") + SettingPersistent::getMcuVersion() + QString("V1.0");
    qDebug() << __PRETTY_FUNCTION__ << __LINE__ << variant.m_Text;
    item->setData(qVariantFromValue(variant), Qt::DisplayRole);
    root->setChild(root->rowCount(), 0, item);

    m_CustomListView->setModel(m_StandardItemModel);
    g_Widget->geometryFit(0, 0, 600, 334, m_CustomListView);

    m_UpdateMCUBtn = new BmpButton(q);
    m_UpdateMCUBtn->setNormalBmpPath(QString(":/Images/SettingSoundSoundBlankNormal.png"));
    m_UpdateMCUBtn->setPressBmpPath(QString(":/Images/SettingSoundSoundBlankPress.png"));
    g_Widget->geometryFit(65, 265, 170, 58, m_UpdateMCUBtn);
    m_UpdateMCUBtn->setText(SourceString::Update_MCU);
    m_UpdateMCUBtn->setVisible(QFile::exists(QString("/media/udisk/mcu.bin")));

    QObject::connect(m_UpdateMCUBtn, ARKSENDER(released()),
                     q,          ARKRECEIVER(onBmpButtonRelease()));
    m_ResetBtn = new BmpButton(q);
    m_ResetBtn->setNormalBmpPath(QString(":/Images/SettingSoundSoundBlankNormal.png"));
    m_ResetBtn->setPressBmpPath(QString(":/Images/SettingSoundSoundBlankPress.png"));
    g_Widget->geometryFit(365, 265, 170, 58, m_ResetBtn);
    m_ResetBtn->setText(SourceString::Reset);
    m_ResetBtn->setVisible(true);
    QObject::connect(m_ResetBtn, ARKSENDER(released()),
                     q,          ARKRECEIVER(onBmpButtonRelease()));
    m_CustomListView->setVisible(true);
}

void SettingGeneralVersionWidgetPrivate::connectAllSlots()
{
    Q_Q(SettingGeneralVersionWidget);
    connectSignalAndSlotByNamesake(g_Setting, q, ARKRECEIVER(onArkProtocol(const ArkProtocolWrapper&)));
    connectSignalAndSlotByNamesake(g_Multimedia, q, ARKRECEIVER(onDeviceWatcherStatus(const int, const int)));
}

void SettingGeneralVersionWidgetPrivate::initializeResetMessageDialog()
{
    if (NULL == m_ResetMessageDialog) {
        Q_Q(SettingGeneralVersionWidget);
        m_ResetMessageDialog = new SettingMessageDialog(q);
        m_ResetMessageDialog->setTitle(SourceString::Reset_Tip);
        m_ResetMessageDialog->setConfirm(SourceString::Confirm);
        m_ResetMessageDialog->setCancel(SourceString::Cancel);
        m_ResetMessageDialog->setVisible(false);
        QObject::connect(m_ResetMessageDialog, ARKSENDER(confirm()),
                         q,                    ARKRECEIVER(onConfirm()));
        QObject::connect(m_ResetMessageDialog, ARKSENDER(cancel()),
                         q,                    ARKRECEIVER(onCancel()));
    }
}

void SettingGeneralVersionWidgetPrivate::initializeUpdateMCUMessageDialog()
{
    if (NULL == m_UpdateMCUMessageDialog) {
        Q_Q(SettingGeneralVersionWidget);
        m_UpdateMCUMessageDialog = new SettingMessageDialog(q);
        m_UpdateMCUMessageDialog->setTitleTranslate(false);
        m_UpdateMCUMessageDialog->setConfirm(SourceString::Confirm);
        m_UpdateMCUMessageDialog->setCancel(SourceString::Cancel);
        m_UpdateMCUMessageDialog->setVisible(false);
        QObject::connect(m_UpdateMCUMessageDialog, ARKSENDER(confirm()),
                         q,                        ARKRECEIVER(onConfirm()));
        QObject::connect(m_UpdateMCUMessageDialog, ARKSENDER(cancel()),
                         q,                        ARKRECEIVER(onCancel()));
    }
}

VersionDelegate::VersionDelegate(QObject *parent)
    : CustomItemDelegate(parent)
{
    QString path(":/Images/MultimediaListViewRules.png");
    if (QFile::exists(path)) {
        m_Rules.reset(new QPixmap(path));
    }
}

VersionDelegate::~VersionDelegate()
{
}

void VersionDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->setFont(QFont(QString(), 11 * g_Widget->widthScalabilityFactor()));
    if (!m_Rules.isNull()) {
        painter->drawPixmap(0, option.rect.y() + option.rect.height() - 2, *m_Rules);
    }
    VersionVariant variant = qVariantValue<VersionVariant>(index.data(Qt::DisplayRole));
    painter->setPen(Qt::white);
    painter->drawText(option.rect, Qt::AlignCenter, variant.m_Text);
}
