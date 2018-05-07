#include "MirrorShortCutWidget.h"
#include "AutoConnect.h"
#include "BusinessLogicUtility.h"
#include "Widget.h"
#include "EventEngine.h"
#include "UserInterfaceUtility.h"
#include <QVariant>
#include <QPainter>

class MirrorShortCutWidgetPrivate
{
    Q_DISABLE_COPY(MirrorShortCutWidgetPrivate)
public:
    explicit MirrorShortCutWidgetPrivate(MirrorShortCutWidget* parent);
    ~MirrorShortCutWidgetPrivate();
    void initialize();
    void receiveAllCustomEvent();
    void connectAllSlots();
    BmpButton* m_Background = NULL;
private:
    MirrorShortCutWidget* m_Parent = NULL;
};

MirrorShortCutWidget::MirrorShortCutWidget(QWidget *parent)
    : QWidget(parent)
    , Link::Interface()
    , m_Private(new MirrorShortCutWidgetPrivate(this))
{
}

MirrorShortCutWidget::~MirrorShortCutWidget()
{
}

void MirrorShortCutWidget::resizeEvent(QResizeEvent *event)
{
    int width(84);
    int height(84);
    g_Widget->geometryFit((104 - width) * 0.5, (g_Widget->baseTabBarHeight() - height) * 0.5, width, height, m_Private->m_Background);
    QWidget::resizeEvent(event);
}

void MirrorShortCutWidget::customEvent(QEvent *event)
{
    qDebug() << "MirrorShortCutWidget::customEvent" << event->type();
    switch (event->type()) {
//    case CustomEventType::TabBarWidgetAddChild: {
//        EventEngine::CustomEvent<QVariant>* ptr = dynamic_cast<EventEngine::CustomEvent<QVariant>*>(event);
//        qDebug() << "!!!!!!test" << ptr << ptr->m_Data << ptr->m_Data->canConvert<QWidget*>() << ptr->m_Data->value<QWidget*>();
//        if ((NULL != ptr)
//                && (NULL != ptr->m_Data)
//                && (ptr->m_Data->canConvert<QWidget*>())
//                && (NULL != ptr->m_Data->value<QWidget*>())) {
//            if (this != ptr->m_Data->value<QWidget*>()) {
//                this->setVisible(false);
//            }
//        }
//        break;
//    }
    default: {
        break;
    }
    }
    QWidget::customEvent(event);
}

void MirrorShortCutWidget::onLinkStatusChange(const Link_Type type, const Link_STATUS status)
{
    qDebug() << "MirrorShortCutWidget::onLinkStatusChange" << type << status;
    switch (type) {
//    case ANDROID_MIRROR: {
//        if (LINK_EXITING == status) {
//            QVariant* variant = new QVariant();
//            variant->setValue(static_cast<QWidget*>(this));
//            EventEngine::CustomEvent<QVariant> event(static_cast<QEvent::Type>(CustomEventType::TabBarWidgetAddChild), variant);
//            g_EventEngine->sendCustomEvent(event);
//            g_Widget->geometryFit(0 + 84 * 3, 0, 104, g_Widget->baseTabBarHeight(), this);
//        } else {
//            setVisible(false);
//        }
//        break;
//    }
    default: {
        break;
    }
    }
}

void MirrorShortCutWidget::onBmpButtonRelease()
{
    g_Link->requestLinkStatus(ANDROID_MIRROR, LINK_STARTING);
}

MirrorShortCutWidgetPrivate::MirrorShortCutWidgetPrivate(MirrorShortCutWidget *parent)
    : m_Parent(parent)
{
    initialize();
    receiveAllCustomEvent();
    connectAllSlots();
}

MirrorShortCutWidgetPrivate::~MirrorShortCutWidgetPrivate()
{
}

void MirrorShortCutWidgetPrivate::initialize()
{
    m_Background = new BmpButton(m_Parent);
    m_Background->show();
    m_Background->setNormalBmpPath(QString(":/Images/Resources/Images/MirrorShortCutWidgetBackground"));
    m_Background->setPressBmpPath(QString(":/Images/Resources/Images/MirrorShortCutWidgetBackground"));
}

void MirrorShortCutWidgetPrivate::receiveAllCustomEvent()
{
    g_EventEngine->attachCustomEvent(m_Parent);
}

void MirrorShortCutWidgetPrivate::connectAllSlots()
{
    connectSignalAndSlotByNamesake(g_Link, m_Parent);
    Qt::ConnectionType type = static_cast<Qt::ConnectionType>(Qt::AutoConnection | Qt::UniqueConnection);
    QObject::connect(m_Background, SIGNAL(bmpButtonRelease()),
                     m_Parent,     SLOT(onBmpButtonRelease()),
                     type);
}
