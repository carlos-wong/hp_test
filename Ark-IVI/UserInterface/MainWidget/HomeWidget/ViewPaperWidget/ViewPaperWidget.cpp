#include "ViewPaperWidget.h"
#include "AutoConnect.h"
#include "UserInterfaceUtility.h"
#include "ViewPaperTipWidget/ViewPaperTipWidget.h"
#include <QEvent>
#include <QPalette>
#include <QPoint>
#include <QPainter>
#include <QMouseEvent>
#include <QScrollBar>
#include <QStandardItemModel>
#include <QDebug>
#include <QPropertyAnimation>
#include <QMouseEvent>
#include "BusinessLogic/Setting.h"

bool g_isClickedTlink = false;

namespace SourceString {
static const QString Radio(QObject::tr("Radio"));
static const QString Bluetooth(QObject::tr("Bluetooth"));
static const QString SD(QObject::tr("SD"));
static const QString USB(QObject::tr("USB"));
static const QString Link(QObject::tr("Link"));
static const QString AUX(QObject::tr("AUX"));
static const QString Camera(QObject::tr("Camera"));
static const QString Steering(QObject::tr("Steering"));
static const QString Setting(QObject::tr("Setting"));
}

class ViewPaperVariant
{
public:
    enum ButtonStatus {
        B_Normal = 0,
        B_Press = 1,
    };
    ViewPaperVariant();
    ~ViewPaperVariant();
    QList<QRect> m_IconRectList;
    QList<QRect> m_TextRectList;
    QList<QPixmap*> m_NormalList;
    QList<QPixmap*> m_PressList;
    QList<QString> m_TextList;
    QList<ViewPaperVariant::ButtonStatus> m_StatusList;
};
Q_DECLARE_METATYPE(ViewPaperVariant)

class ViewPaperItemDelegate;
class ViewPaperWidgetPrivate
{
    Q_DISABLE_COPY(ViewPaperWidgetPrivate)
public:
    explicit ViewPaperWidgetPrivate(ViewPaperWidget* parent);
    ~ViewPaperWidgetPrivate();
    void initializeParent();
    void initializeBasic();
    void initializeBasicWidget();
    void initializePropertyAnimation();
    void initializeFirstView();
    void initializeSecondView();
    void firstViewPaperHandler(const unsigned int index);
    void secondViewPaperHandler(const unsigned int index);
private:
    Q_DECLARE_PUBLIC(ViewPaperWidget)
    ViewPaperWidget* const q_ptr;
    ViewPaperTipWidget* m_ViewPaperTipWidget;
    ViewPaperItemDelegate* m_ViewPaperItemDelegate;
    QStandardItemModel* m_StandardItemModel;
    bool m_Finish;
    bool m_Filter;
    QPoint m_StartMovePoint;
    unsigned short m_Threshold;
    ViewPaperVariant m_FirstViewPaperVariant;
    ViewPaperVariant m_SecondViewPaperVariant;
    QPropertyAnimation m_PropertyAnimation;
    unsigned short m_CurrentViewPapger;
};

ViewPaperWidget::ViewPaperWidget(QWidget *parent)
    : QListView(parent)
    , d_ptr(new ViewPaperWidgetPrivate(this))
{
}

ViewPaperWidget::~ViewPaperWidget()
{
    if (NULL != d_ptr) {
        delete d_ptr;
    }
}

void ViewPaperWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(0, 250, 800, 350, this);
}

void ViewPaperWidget::mousePressEvent(QMouseEvent *event)
{
    Q_D(ViewPaperWidget);
    d->initializePropertyAnimation();
    d->initializeSecondView();
    if (d->m_Finish) {
        if (rect().contains(event->pos())) {
            d->m_StartMovePoint = event->pos();
            d->m_Filter = false;
            QModelIndex modelIndex = indexAt(event->pos());
            if (modelIndex.isValid()) {
                update(modelIndex);
            }
        }
        QListView::mousePressEvent(event);
    }
}

void ViewPaperWidget::mouseMoveEvent(QMouseEvent *event)
{
    Q_D(ViewPaperWidget);
    if (d->m_Finish) {
        if (rect().contains(event->pos())) {
            QPoint relativePos = d->m_StartMovePoint - event->pos();
            int deltaEnd = event->pos().x() - d->m_StartMovePoint.x();
            if (d->m_Filter) {
                d->m_StartMovePoint = event->pos();
                horizontalScrollBar()->setValue(horizontalOffset() + relativePos.x());
            } else if ((qAbs(deltaEnd) > d->m_Threshold)) {
                d->m_Filter = true;
                d->m_StartMovePoint = event->pos();
                if (relativePos.x() > 0) {
                    horizontalScrollBar()->setValue(horizontalOffset() + relativePos.x() - d->m_Threshold);
                } else {
                    horizontalScrollBar()->setValue(horizontalOffset() + relativePos.x() + d->m_Threshold);
                }
            }
        }
        QListView::mouseReleaseEvent(event);
    }
}

void ViewPaperWidget::mouseReleaseEvent(QMouseEvent *event)
{
    Q_D(ViewPaperWidget);
    if (d->m_Finish) {
        int width = 800 * g_Widget->widthScalabilityFactor();
//        int distance = 200 *  g_Widget->widthScalabilityFactor();
        int distance = 80 *  g_Widget->widthScalabilityFactor();
        int offset = horizontalOffset() % width;
        if (0 != offset) {
            if (offset < distance) {
                d->m_PropertyAnimation.setStartValue(horizontalOffset());
                d->m_PropertyAnimation.setEndValue(0);
                d->m_CurrentViewPapger = 0;
                d->m_Finish = false;
                d->m_PropertyAnimation.start();
            } else if ((offset >= distance) && (offset <= (width - distance))) {
                d->m_PropertyAnimation.setStartValue(horizontalOffset());
                if (0 == d->m_CurrentViewPapger) {
                    d->m_PropertyAnimation.setEndValue(width);
                    d->m_CurrentViewPapger = 1;
                } else if (1 == d->m_CurrentViewPapger) {
                    d->m_PropertyAnimation.setEndValue(0);
                    d->m_CurrentViewPapger = 0;
                }
                d->m_Finish = false;
                d->m_PropertyAnimation.start();
            } else if (offset > (width - distance)) {
                d->m_PropertyAnimation.setStartValue(horizontalOffset());
                d->m_PropertyAnimation.setEndValue(width);
                d->m_CurrentViewPapger = 1;
                d->m_Finish = false;
                d->m_PropertyAnimation.start();
            }
        }
        d->m_Filter = false;
        QListView::mouseReleaseEvent(event);
    }
}

void ViewPaperWidget::setHorizontalOffset(int value)
{
    QScrollBar* scrollBar = horizontalScrollBar();
    if (NULL != scrollBar) {
        if (value % width() == 0) {
            Q_D(ViewPaperWidget);
            d->m_Finish = true;
        }
        scrollBar->setValue(value);
    }
}

void ViewPaperWidget::onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status)
{
    switch (destinationType) {
    case Widget::T_ViewPaper0: {
        if (WidgetStatus::RequestShow == status) {
            g_Widget->setWidgetType(Widget::T_ViewPaper0, Widget::T_Undefine, WidgetStatus::Show);
        } else if (WidgetStatus::Show == status) {
            if (0 != horizontalOffset()) {
                horizontalScrollBar()->setValue(0);
            }
            Q_D(ViewPaperWidget);
            d->m_CurrentViewPapger = 0;
            d->m_Finish = true;
            if (!isVisible()) {
                setVisible(true);
            }
        }
        break;
    }
    case Widget::T_ViewPaper1: {
        if (WidgetStatus::RequestShow == status) {
            g_Widget->setWidgetType(Widget::T_ViewPaper1, Widget::T_Undefine, WidgetStatus::Show);
        } else if (WidgetStatus::Show == status) {
            if ((800 * g_Widget->widthScalabilityFactor()) != horizontalOffset()) {
                horizontalScrollBar()->setValue(800 * g_Widget->widthScalabilityFactor());
            }
            Q_D(ViewPaperWidget);
            d->m_CurrentViewPapger = 1;
            d->m_Finish = true;
            if (!isVisible()) {
                setVisible(true);
            }
        }
        break;
    }
    default: {
        break;
    }
    }
}

void ViewPaperWidget::onFinished()
{
    Q_D(ViewPaperWidget);
    switch (d->m_CurrentViewPapger) {
    case 0: {
        g_Widget->setWidgetType(Widget::T_ViewPaper0, Widget::T_ViewPaper1, WidgetStatus::RequestShow);
        break;
    }
    case 1: {
        g_Widget->setWidgetType(Widget::T_ViewPaper1, Widget::T_ViewPaper0, WidgetStatus::RequestShow);
        break;
    }
    default: {
        break;
    }
    }
}

void ViewPaperWidget::onButtonRelease(const unsigned int index)
{
    Q_D(ViewPaperWidget);
    if (isVisible()) {
        switch (d->m_CurrentViewPapger) {
        case 0: {
            d->firstViewPaperHandler(index);
            break;
        }
        case 1: {
            d->secondViewPaperHandler(index);
            break;
        }
        default: {
            break;
        }
        }
    }
}

ViewPaperWidgetPrivate::ViewPaperWidgetPrivate(ViewPaperWidget *parent)
    : q_ptr(parent)
{
    m_ViewPaperTipWidget = NULL;
    m_ViewPaperItemDelegate = NULL;
    m_StandardItemModel = NULL;
    m_Finish = true;
    m_Filter = false;
    m_StartMovePoint = QPoint(0, 0);
    m_Threshold = 10;
    m_FirstViewPaperVariant;
    m_SecondViewPaperVariant;
    m_PropertyAnimation;
    m_CurrentViewPapger = -1;
    initializeBasic();
    initializeBasicWidget();
    initializeParent();
}

ViewPaperWidgetPrivate::~ViewPaperWidgetPrivate()
{
    for (QList<QPixmap*>::Iterator iter = m_FirstViewPaperVariant.m_NormalList.begin();
         iter != m_FirstViewPaperVariant.m_NormalList.end(); ++iter) {
        delete *iter;
    }
    for (QList<QPixmap*>::Iterator iter = m_FirstViewPaperVariant.m_PressList.begin();
         iter != m_FirstViewPaperVariant.m_PressList.end(); ++iter) {
        delete *iter;
    }
}

void ViewPaperWidgetPrivate::initializeParent()
{
    Q_Q(ViewPaperWidget);
    QPalette palette = q->palette();
    palette.setBrush(QPalette::Base, Qt::NoBrush);
    q->setPalette(palette);
    q->setFlow(QListView::LeftToRight);
    q->setFrameShape(QListView::NoFrame);
    q->setHorizontalScrollMode(QListView::ScrollPerPixel);
    q->setEditTriggers(QListView::NoEditTriggers);
    q->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    q->setItemDelegate(m_ViewPaperItemDelegate);
    Qt::ConnectionType type = static_cast<Qt::ConnectionType>(Qt::UniqueConnection | Qt::AutoConnection);
    QObject::connect(m_ViewPaperItemDelegate, ARKSENDER(buttonRelease(const unsigned int)),
                     q,                       ARKRECEIVER(onButtonRelease(const unsigned int)),
                     type);
    q->setModel(m_StandardItemModel);
    q->setVisible(false);
}

void ViewPaperWidgetPrivate::initializeBasic()
{
    Q_Q(ViewPaperWidget);
    connectSignalAndSlotByNamesake(g_Widget, q, ARKRECEIVER(onWidgetTypeChange(const Widget::Type, const Widget::Type, const QString &)));
    if (NULL == m_StandardItemModel) {
        m_StandardItemModel = new QStandardItemModel(q);
    }
    initializeFirstView();
}

void ViewPaperWidgetPrivate::initializeBasicWidget()
{
    Q_Q(ViewPaperWidget);
    if (NULL == m_ViewPaperTipWidget) {
        m_ViewPaperTipWidget = new ViewPaperTipWidget(q);
    }
    if (NULL == m_ViewPaperItemDelegate) {
        m_ViewPaperItemDelegate = new ViewPaperItemDelegate(q);
    }
}

void ViewPaperWidgetPrivate::initializePropertyAnimation()
{
    static bool flag(true);
    if (flag) {
        flag = false;
        Q_Q(ViewPaperWidget);
        m_PropertyAnimation.setTargetObject(q);
        m_PropertyAnimation.setPropertyName(QByteArray("horizontalOffset"));
        Qt::ConnectionType type = static_cast<Qt::ConnectionType>(Qt::UniqueConnection | Qt::AutoConnection);
        QObject::connect(&m_PropertyAnimation, ARKSENDER(finished()),
                         q,                    ARKRECEIVER(onFinished()),
                         type);
    }
}

void ViewPaperWidgetPrivate::initializeFirstView()
{
    static bool flag(true);
    if (flag) {
        flag = false;
        Q_Q(ViewPaperWidget);
        m_CurrentViewPapger = 0;

        QStringList btnNameList;
        btnNameList << "Radio" << "Carlife" << "Carplay" << "SD" << "USB";

        QStringList btnImageNormalList;
        btnImageNormalList << ":/Images/hw_01_radio_normal.png" << ":/Images/hw_01_carlife_normal.png" << ":/Images/hw_01_carplay_normal.png"
                           << ":/Images/hw_01_sd_normal.png" << ":/Images/hw_01_usb_normal.png";

        QStringList btnImagePressedList;
        btnImagePressedList << ":/Images/hw_01_radio_pressed.png" << ":/Images/hw_01_carlife_pressed.png" << ":/Images/hw_01_carplay_pressed.png"
                           << ":/Images/hw_01_sd_pressed.png" << ":/Images/hw_01_usb_pressed.png";

        m_FirstViewPaperVariant.m_StatusList.clear();
        m_FirstViewPaperVariant.m_IconRectList.clear();
        m_FirstViewPaperVariant.m_TextRectList.clear();
        m_FirstViewPaperVariant.m_TextList.clear();
        m_FirstViewPaperVariant.m_NormalList.clear();
        m_FirstViewPaperVariant.m_PressList.clear();

        int len = btnNameList.length();
        for (int i = 0; i < len; ++i) {
            m_FirstViewPaperVariant.m_StatusList.append(ViewPaperVariant::B_Normal);
            m_FirstViewPaperVariant.m_IconRectList.append(g_Widget->geometryFit(70 + (45 + 96) * i, 37, 106, 106));
            m_FirstViewPaperVariant.m_TextRectList.append(g_Widget->geometryFit(62 + (45 + 96) * i, 178 - 55, 106, 65));
            m_FirstViewPaperVariant.m_TextList.append(btnNameList[i]);
            m_FirstViewPaperVariant.m_NormalList.append(new QPixmap(btnImageNormalList[i]));
            m_FirstViewPaperVariant.m_PressList.append(new QPixmap(btnImagePressedList[i]));
        }

        QStandardItem* firstItem = new QStandardItem();
        firstItem->setSizeHint(QSize(800 * g_Widget->widthScalabilityFactor(), 178 * g_Widget->heightScalabilityFactor()));
        firstItem->setData(qVariantFromValue(m_FirstViewPaperVariant), Qt::UserRole);
        QStandardItem* root = m_StandardItemModel->invisibleRootItem();
        m_StandardItemModel->setItem(root->rowCount(), 0, firstItem);
    }
}

void ViewPaperWidgetPrivate::initializeSecondView()
{
    static bool flag(true);
    if (flag) {
        flag = false;
        Q_Q(ViewPaperWidget);

        QStringList btnNameList;
        btnNameList << "Bluetooth" << "AVIN" << "Steering" << "Setting" ;

        QStringList btnImageNormalList;
        btnImageNormalList << ":/Images/hw_01_bluetooth_normal.png" << ":/Images/hw_01_avin1_normal.png"
                           << ":/Images/hw_01_control_car_normal.png" << ":/Images/hw_01_setup_normal.png";

        QStringList btnImagePressedList;
        btnImagePressedList << ":/Images/hw_01_bluetooth_pressed.png" << ":/Images/hw_01_avin1_pressed.png"
                           << ":/Images/hw_01_control_car_pressed.png" << ":/Images/hw_01_setup_pressed.png";

        m_SecondViewPaperVariant.m_StatusList.clear();
        m_SecondViewPaperVariant.m_IconRectList.clear();
        m_SecondViewPaperVariant.m_TextRectList.clear();
        m_SecondViewPaperVariant.m_TextList.clear();
        m_SecondViewPaperVariant.m_NormalList.clear();
        m_SecondViewPaperVariant.m_PressList.clear();

        int len = btnNameList.length();
        for (int i = 0; i < len; ++i) {
            m_SecondViewPaperVariant.m_StatusList.append(ViewPaperVariant::B_Normal);
            m_SecondViewPaperVariant.m_IconRectList.append(g_Widget->geometryFit(70 + (45 + 96) * i, 37, 106, 106));
            m_SecondViewPaperVariant.m_TextRectList.append(g_Widget->geometryFit(62 + (45 + 96) * i, 178 - 55, 106, 65));
            m_SecondViewPaperVariant.m_TextList.append(btnNameList[i]);
            m_SecondViewPaperVariant.m_NormalList.append(new QPixmap(btnImageNormalList[i]));
            m_SecondViewPaperVariant.m_PressList.append(new QPixmap(btnImagePressedList[i]));
        }

        QStandardItem* secondItem = new QStandardItem();
        secondItem->setSizeHint(QSize(800 * g_Widget->widthScalabilityFactor(), 178 * g_Widget->heightScalabilityFactor()));
        secondItem->setData(qVariantFromValue(m_SecondViewPaperVariant), Qt::UserRole);
        QStandardItem* root = m_StandardItemModel->invisibleRootItem();
        m_StandardItemModel->setItem(root->rowCount(), 0, secondItem);
    }
}

void ViewPaperWidgetPrivate::firstViewPaperHandler(const unsigned int index)
{
    switch (index) {
    case 0: {
        g_Widget->setWidgetType(Widget::T_Radio, Widget::T_ViewPaper0, WidgetStatus::RequestShow);
        break;
    }
    case 1: {
        g_isClickedTlink = true;
//        int type = SettingPersistent::getPhoneLinkType();
        int type = 0;
        if  (type == -1) type = 0;
        if  (type == 0) {
            g_Widget->setWidgetType(Widget::T_Link, Widget::T_Carlife, WidgetStatus::RequestShow);
        }
        else if  (type == 1) {
            g_Widget->setWidgetType(Widget::T_Link, Widget::T_Auto, WidgetStatus::RequestShow);
        }
        break;
    }
    case 2: {
        g_isClickedTlink = true;
        g_Widget->setWidgetType(Widget::T_Link, Widget::T_Carplay, WidgetStatus::RequestShow);
        break;
    }
    case 3: {
        g_Widget->setWidgetType(Widget::T_SDDisk, Widget::T_ViewPaper0, WidgetStatus::RequestShow);
        break;
    }
    case 4: {
        g_Widget->setWidgetType(Widget::T_USBDisk, Widget::T_ViewPaper0, WidgetStatus::RequestShow);
        break;
    }
    default: {
        break;
    }
    }
}

void ViewPaperWidgetPrivate::secondViewPaperHandler(const unsigned int index)
{
    switch (index) {
    case 0: {
        g_Widget->setWidgetType(Widget::T_Bluetooth, Widget::T_ViewPaper1, WidgetStatus::RequestShow);
        break;
    }
    case 1: {
//        g_Widget->setWidgetType(Widget::T_Camera, Widget::T_ViewPaper1, WidgetStatus::RequestShow);
        g_Widget->setWidgetType(Widget::T_AUX, Widget::T_ViewPaper1, WidgetStatus::RequestShow);
        break;
    }
    case 2: {
        g_Widget->setWidgetType(Widget::T_Steering, Widget::T_ViewPaper1, WidgetStatus::RequestShow);
        break;
    }
    case 3: {
        g_Widget->setWidgetType(Widget::T_Setting, Widget::T_ViewPaper1, WidgetStatus::RequestShow);
        break;
    }
    default: {
        break;
    }
    }
}

ViewPaperItemDelegate::ViewPaperItemDelegate(QObject *parent)
    : CustomItemDelegate(parent)
{
    m_StartMovePoint = QPoint(0, 0);
    m_Filter = false;
}

ViewPaperItemDelegate::~ViewPaperItemDelegate()
{
}

void ViewPaperItemDelegate::mousePressEvent(QMouseEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    ViewPaperVariant variant = qVariantValue<ViewPaperVariant>(index.data(Qt::UserRole));
    for (int i = 0; i < variant.m_IconRectList.size(); ++i) {
        QRect rect(variant.m_IconRectList.at(i).x() + option.rect.x(),
                   variant.m_IconRectList.at(i).y() + option.rect.y(),
                   variant.m_IconRectList.at(i).width(),
                   variant.m_IconRectList.at(i).height());
        if (variant.m_StatusList.size() > i) {
            if (rect.contains(event->pos())) {
                m_StartMovePoint = event->pos();
                m_Filter = false;
                variant.m_StatusList.replace(i, ViewPaperVariant::B_Press);
                model->setData(index, qVariantFromValue(variant), Qt::UserRole);
                break;
            }
        }
    }
}

void ViewPaperItemDelegate::mouseMoveEvent(QMouseEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    if (!m_Filter) {
        int deltaEnd = event->pos().x() - m_StartMovePoint.x();
        if (qAbs(deltaEnd) > 10) {
            m_Filter = true;
            m_StartMovePoint = QPoint(0, 0);
            ViewPaperVariant variant = qVariantValue<ViewPaperVariant>(index.data(Qt::UserRole));
            for (int i = 0; i < variant.m_StatusList.size(); ++i) {
                if (variant.m_StatusList.size() > i) {
                    if (ViewPaperVariant::B_Normal != variant.m_StatusList.at(i)) {
                        variant.m_StatusList.replace(i, ViewPaperVariant::B_Normal);
                        model->setData(index, qVariantFromValue(variant), Qt::UserRole);
                        break;
                    }
                }
            }
        }
    }
}

void ViewPaperItemDelegate::mouseReleaseEvent(QMouseEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    ViewPaperVariant variant = qVariantValue<ViewPaperVariant>(index.data(Qt::UserRole));
    for (int i = 0; i < variant.m_IconRectList.size(); ++i) {
        if (variant.m_StatusList.size() > i) {
            if (ViewPaperVariant::B_Press == variant.m_StatusList.at(i)) {
                buttonRelease(i);
            }
            variant.m_StatusList.replace(i, ViewPaperVariant::B_Normal);
            model->setData(index, qVariantFromValue(variant), Qt::UserRole);
        }
    }
}

void ViewPaperItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->setPen(Qt::white);
    ViewPaperVariant variant = qVariantValue<ViewPaperVariant>(index.data(Qt::UserRole));
    for (int i = 0; i < variant.m_StatusList.size(); ++i) {
        switch (variant.m_StatusList.at(i)) {
        case ViewPaperVariant::B_Normal: {
            if ((variant.m_NormalList.size() > i)
                    && (NULL != variant.m_NormalList.at(i))) {
                QRect normalRect(variant.m_IconRectList.at(i).x() + option.rect.x(),
                                 variant.m_IconRectList.at(i).y() + option.rect.y(),
                                 variant.m_IconRectList.at(i).width(),
                                 variant.m_IconRectList.at(i).height());
                painter->drawPixmap(normalRect.x(), normalRect.y(), *variant.m_NormalList.at(i));
            }
            break;
        }
        case ViewPaperVariant::B_Press: {
            if ((variant.m_PressList.size() > i)
                    && (NULL != variant.m_PressList.at(i))) {
                QRect pressRect(variant.m_IconRectList.at(i).x() + option.rect.x(),
                                variant.m_IconRectList.at(i).y() + option.rect.y(),
                                variant.m_IconRectList.at(i).width(),
                                variant.m_IconRectList.at(i).height());
                painter->drawPixmap(pressRect.x(), pressRect.y(), *variant.m_PressList.at(i));
            }
            break;
        }
        default: {
            break;
        }
        }
        if (variant.m_TextRectList.size() > i) {
            QRect textRect(variant.m_TextRectList.at(i).x() + option.rect.x(),
                           variant.m_TextRectList.at(i).y() + option.rect.y(),
                           variant.m_TextRectList.at(i).width(),
                           variant.m_TextRectList.at(i).height());
            painter->drawText(textRect, Qt::AlignCenter, QObject::tr(variant.m_TextList.at(i).toLocal8Bit().constData()));
        }
    }
}

ViewPaperVariant::ViewPaperVariant()
{
}

ViewPaperVariant::~ViewPaperVariant()
{
}
