#include "MirrorLinkWidget.h"
#include "AutoConnect.h"
#include "UserInterfaceUtility.h"
#include "BusinessLogic/BusinessLogicUtility.h"
#include "BmpButton.h"
#include "BmpWidget.h"
#include "BusinessLogic/Setting.h"
#include "MirrorToolWidget/MirrorToolWidget.h"
#include "MirrorMessageDialog/MirrorMessageDialog.h"
#include "UserInterfaceUtility.h"
#include <QMouseEvent>
#include <QTimer>
#include <QPainter>
#include <QList>
#include <QTouchEvent>
#include <QDomElement>

class MirrorLinkWidgetPrivate
{
    Q_DISABLE_COPY(MirrorLinkWidgetPrivate)
public:
    explicit MirrorLinkWidgetPrivate(MirrorLinkWidget* parent);
    ~MirrorLinkWidgetPrivate();
    void initializePopBtn();
    void initializeToolWidget();
    void initializeTimer();
    void initializeBluetoothTipWidget();
    void initializeMessageBox();
    void connectAllSlots();
    void onMirrorLinkStatus(const Link_STATUS status);
    void touchBeginEventHandler(QEvent* event);
    void touchUpdateEventHandler(QEvent* event);
    void touchEndEventHandler(QEvent* event);
    QString translateTouchEvent(const QList<TouchEvent> &list);
    MirrorMessageDialog* m_MirrorMessageDialog = NULL;
    MirrorToolWidget* m_MirrorToolWidget = NULL;
    BmpButton* m_PopBtn = NULL;
    BmpWidget* m_BluetoothTip = NULL;
    bool m_Filter = false;
    QTimer* m_Timer = NULL;
private:
    MirrorLinkWidget* m_Parent = NULL;
};

MirrorLinkWidget::MirrorLinkWidget(QWidget *parent)
    : QWidget(parent)
    , m_Private(new MirrorLinkWidgetPrivate(this))
{
}

MirrorLinkWidget::~MirrorLinkWidget()
{
}

void MirrorLinkWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(0, 0, 800, 480, this);
    //    g_Widget->geometryFit(0, 0, g_Widget->baseWindowWidth(), g_Widget->baseWindowHeight(), m_Private->m_MirrorMessageDialog);
}

void MirrorLinkWidget::mousePressEvent(QMouseEvent *event)
{
    qWarning() << __FUNCTION__ << event->pos() << qgetenv("QWS_ARK_MT_DEVICE").isEmpty();
    if (qgetenv("QWS_ARK_MT_DEVICE").isEmpty()) {
        QList<TouchEvent> touchPointList;
        touchPointList.clear();
        TouchEvent touchEvent;
        touchEvent.state = TouchEvent::TouchPointPressed;
        touchEvent.x = event->pos().x();
        touchEvent.y = event->pos().y();
        touchPointList.append(touchEvent);
        QString touchPointXml = m_Private->translateTouchEvent(touchPointList);
        g_Link->requestTouchStatus(ANDROID_MIRROR, TouchBegin, touchPointXml);
    }
}

void MirrorLinkWidget::mouseReleaseEvent(QMouseEvent *event)
{
    //    qWarning() << __FUNCTION__ << event->pos() << qgetenv("QWS_ARK_MT_DEVICE").isEmpty();
    if (qgetenv("QWS_ARK_MT_DEVICE").isEmpty()) {
        QList<TouchEvent> touchPointList;
        touchPointList.clear();
        TouchEvent touchEvent;
        touchEvent.state = TouchEvent::TouchPointReleased;
        touchEvent.x = event->pos().x();
        touchEvent.y = event->pos().y();
        touchPointList.append(touchEvent);
        QString touchPointXml = m_Private->translateTouchEvent(touchPointList);
        g_Link->requestTouchStatus(ANDROID_MIRROR, TouchBegin, touchPointXml);
    }
}

void MirrorLinkWidget::mouseMoveEvent(QMouseEvent *event)
{
    //    qWarning() << __FUNCTION__ << event->pos() << QString(qgetenv("QWS_ARK_MT_DEVICE").data()).isEmpty();
    if (qgetenv("QWS_ARK_MT_DEVICE").isEmpty()) {
        QList<TouchEvent> touchPointList;
        touchPointList.clear();
        TouchEvent touchEvent;
        touchEvent.state = TouchEvent::TouchPointMoved;
        touchEvent.x = event->pos().x();
        touchEvent.y = event->pos().y();
        touchPointList.append(touchEvent);
        QString touchPointXml = m_Private->translateTouchEvent(touchPointList);
        g_Link->requestTouchStatus(ANDROID_MIRROR, TouchBegin, touchPointXml);
    }
}

bool MirrorLinkWidget::event(QEvent *event)
{
    bool ret(true);
    switch (event->type()) {
    case QEvent::TouchBegin:
        m_Private->touchBeginEventHandler(event);
        ret = true;
        break;
    case QEvent::TouchUpdate: {
        m_Private->touchUpdateEventHandler(event);
        return true;
        break;
    }
    case QEvent::TouchEnd: {
        m_Private->touchEndEventHandler(event);
        ret = true;
        break;
    }
    default : {
        ret = QWidget::event(event);
        break;
    }
    }
    return ret;
}

void MirrorLinkWidget::onWidgetTypeChange(const Widget::Type desitinationType, const Widget::Type requestType, const QString &status)
{
    switch (desitinationType) {
    case Widget::T_Mirror: {
        if (WidgetStatus::RequestShow == status) {
            g_Link->requestLinkStatus(ANDROID_MIRROR, LINK_STARTING);
        } else if (WidgetStatus::Show == status) {
            raise();
            if (!qgetenv("QWS_ARK_MT_DEVICE").isEmpty()) {
                setAttribute(Qt::WA_AcceptTouchEvents, true);
            }
            m_Private->initializeTimer();
//            m_Private->m_Timer->start(5000);
//            m_Private->initializeMessageBox();
            m_Private->initializeToolWidget();
            m_Private->initializePopBtn();
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

void MirrorLinkWidget::onLinkStatusChange(const Link_Type type, const Link_STATUS status)
{
    switch (type) {
    case ANDROID_MIRROR: {
        if (LINK_SUCCESS == status) {
            g_Widget->setWidgetType(Widget::T_Mirror, Widget::T_Link, WidgetStatus::Show);
        }
        break;
    }
    default: {
        break;
    }
    }
}

void MirrorLinkWidget::onPopBtnRelease()
{
    qWarning() << "MirrorLinkWidget::onPopBtnRelease" << m_Private->m_MirrorToolWidget->isVisible();
    if (m_Private->m_MirrorToolWidget->isVisible()) {
        m_Private->m_Timer->stop();
//        m_Private->m_MirrorMessageDialog->setVisible(false);
        m_Private->m_MirrorToolWidget->setVisible(false);
    } else {
        m_Private->m_Timer->start();
        m_Private->m_MirrorToolWidget->setVisible(true);
    }
}

void MirrorLinkWidget::onToolBtnRelease(const int type)
{
    qWarning() << "MirrorLinkWidget::onToolBtnRelease" << type;
    switch(type) {
    case MirrorToolWidget::T_BlueTooth: {
//        if (m_Private->m_MirrorMessageDialog->isVisible()) {
//            m_Private->m_Timer->start();
//            m_Private->m_MirrorMessageDialog->setVisible(false);
//        } else {
//            m_Private->m_MirrorMessageDialog->setVisible(true);
//            m_Private->m_Timer->stop();
//        }
        break;
    }
    case MirrorToolWidget::T_Car: {
        //        m_Private->m_Timer->stop();
        //        m_Private->m_MirrorToolWidget->setVisible(false);
        g_Link->requestLinkStatus(ANDROID_MIRROR, LINK_EXITING);
        break;
    }
    case MirrorToolWidget::T_Back: {
        m_Private->m_Timer->start();
        g_Link->requestKeyValue(BACK_KEY);
        break;
    }
    case MirrorToolWidget::T_Home: {
        m_Private->m_Timer->start();
        g_Link->requestKeyValue(HOME_KEY);
        break;;
    }
    case MirrorToolWidget::T_Menu: {
        m_Private->m_Timer->start();
        g_Link->requestKeyValue(MENU_KEY);
        break;
    }
    default: {
        break;
    }
    }
}

void MirrorLinkWidget::onTimeout()
{
//    m_Private->m_MirrorMessageDialog->setVisible(false);
    m_Private->m_MirrorToolWidget->setVisible(false);
}

MirrorLinkWidgetPrivate::MirrorLinkWidgetPrivate(MirrorLinkWidget* parent)
    : m_Parent(parent)
{
    connectSignalAndSlotByNamesake(g_Widget, m_Parent, SLOT(onWidgetTypeChange(const Widget::Type, const Widget::Type, const QString &)));
    connectSignalAndSlotByNamesake(g_Link, m_Parent, SLOT(onLinkStatusChange(const int, const int)));
}

MirrorLinkWidgetPrivate::~MirrorLinkWidgetPrivate()
{
}

void MirrorLinkWidgetPrivate::initializePopBtn()
{
    if (NULL == m_PopBtn) {
        m_PopBtn = new BmpButton(m_Parent);
        m_PopBtn->setNormalBmpPath(QString(":/Images/LinkWidgetMirrorPotBtnNormal.png"));
        m_PopBtn->setPressBmpPath(QString(":/Images/LinkWidgetMirrorPotBtnPress.png"));
        g_Widget->geometryFit(0, 404, 76, 52, m_PopBtn);
        m_PopBtn->setVisible(true);
        Qt::ConnectionType type = static_cast<Qt::ConnectionType>(Qt::AutoConnection | Qt::UniqueConnection);
        if (NULL != m_PopBtn) {
            QObject::connect(m_PopBtn, SIGNAL(released()),
                             m_Parent, SLOT(onPopBtnRelease()),
                             type);
        }
    }
}

void MirrorLinkWidgetPrivate::initializeToolWidget()
{
    if (NULL == m_MirrorToolWidget) {
        m_MirrorToolWidget = new MirrorToolWidget(m_Parent);
        m_MirrorToolWidget->setVisible(true);
        Qt::ConnectionType type = static_cast<Qt::ConnectionType>(Qt::AutoConnection | Qt::UniqueConnection);
        if (NULL != m_MirrorToolWidget) {
            QObject::connect(m_MirrorToolWidget,  SIGNAL(toolBtnRelease(const int)),
                             m_Parent,            SLOT(onToolBtnRelease(const int)),
                             type);
        }
    }
}

void MirrorLinkWidgetPrivate::initializeTimer()
{
    if (NULL == m_Timer) {
        m_Timer = new QTimer(m_Parent);
        m_Timer->setSingleShot(true);
        m_Timer->setInterval(5000);
        Qt::ConnectionType type = static_cast<Qt::ConnectionType>(Qt::AutoConnection | Qt::UniqueConnection);
        if (NULL != m_Timer) {
            QObject::connect(m_Timer,  SIGNAL(timeout()),
                             m_Parent, SLOT(onTimeout()),
                             type);
        }
    }
}

void MirrorLinkWidgetPrivate::initializeBluetoothTipWidget()
{
    if (NULL == m_BluetoothTip) {
        m_BluetoothTip = new BmpWidget(m_Parent);
        m_BluetoothTip->setVisible(true);
    }
}

void MirrorLinkWidgetPrivate::initializeMessageBox()
{
//    if (NULL == m_MirrorMessageDialog) {
//        m_MirrorMessageDialog = new MirrorMessageDialog(m_Parent);
//        m_MirrorMessageDialog->setVisible(true);
//        Qt::ConnectionType type = static_cast<Qt::ConnectionType>(Qt::AutoConnection | Qt::UniqueConnection);
//        if (NULL != m_MirrorToolWidget) {
//            QObject::connect(m_MirrorToolWidget,  SIGNAL(toolBtnRelease(const int)),
//                             m_Parent,            SLOT(onToolBtnRelease(const int)),
//                             type);
//        }
//    }
}

void MirrorLinkWidgetPrivate::connectAllSlots()
{    
    connectSignalAndSlotByNamesake(g_Link, m_Parent, SLOT(onLinkStatusChange(const int, const int)));
}

void MirrorLinkWidgetPrivate::touchBeginEventHandler(QEvent *event)
{
    QTouchEvent* ptr = static_cast<QTouchEvent*>(event);
    if (NULL != ptr) {
        //qWarning() << __FUNCTION__ << ptr->touchPointStates();
        QList<QTouchEvent::TouchPoint> touchPoints = ptr->touchPoints();
        QList<TouchEvent> touchPointList;
        touchPointList.clear();
        for (int i = 0; i < touchPoints.size(); ++i) {
            //            qWarning() << i << touchPoints.at(i).state() << touchPoints.at(i).pos().toPoint();
            TouchEvent touchEvent;
            touchEvent.state = static_cast<TouchEvent::TouchPointState>(touchPoints.at(i).state());
            touchEvent.x = touchPoints.at(i).pos().toPoint().x();
            touchEvent.y = touchPoints.at(i).pos().toPoint().y();
            touchPointList.append(touchEvent);
        }
        if (touchPointList.size() != 0) {
            QString touchPointXml = translateTouchEvent(touchPointList);
            g_Link->requestTouchStatus(ANDROID_MIRROR, TouchBegin, touchPointXml);
        }
    }
}

void MirrorLinkWidgetPrivate::touchUpdateEventHandler(QEvent *event)
{
    QTouchEvent* ptr = static_cast<QTouchEvent*>(event);
    if (NULL != ptr) {
        //        qWarning() << __FUNCTION__ << ptr->touchPointStates();
        QList<QTouchEvent::TouchPoint> touchPoints = ptr->touchPoints();
        QList<TouchEvent> touchPointList;
        touchPointList.clear();
        for (int i = 0; i < touchPoints.size(); ++i) {
            //            qWarning() << i << touchPoints.at(i).state() << touchPoints.at(i).pos();
            TouchEvent touchEvent;
            touchEvent.state = static_cast<TouchEvent::TouchPointState>(touchPoints.at(i).state());
            touchEvent.x = touchPoints.at(i).pos().toPoint().x();
            touchEvent.y = touchPoints.at(i).pos().toPoint().y();
            touchPointList.append(touchEvent);
        }
        if (touchPointList.size() != 0) {
            QString touchPointXml = translateTouchEvent(touchPointList);
            g_Link->requestTouchStatus(ANDROID_MIRROR, TouchUpdate, touchPointXml);
        }
    }
}

void MirrorLinkWidgetPrivate::touchEndEventHandler(QEvent *event)
{
    QTouchEvent* ptr = static_cast<QTouchEvent*>(event);
    if (NULL != ptr) {
        //        qWarning() << __FUNCTION__ << ptr->touchPointStates();
        QList<QTouchEvent::TouchPoint> touchPoints = ptr->touchPoints();
        QList<TouchEvent> touchPointList;
        touchPointList.clear();
        for (int i = 0; i < touchPoints.size(); ++i) {
            //            qWarning() << i << touchPoints.at(i).state() << touchPoints.at(i).pos();
            TouchEvent touchEvent;
            touchEvent.state = static_cast<TouchEvent::TouchPointState>(touchPoints.at(i).state());
            touchEvent.x = touchPoints.at(i).pos().toPoint().x();
            touchEvent.y = touchPoints.at(i).pos().toPoint().y();
            touchPointList.append(touchEvent);
        }
        if (touchPointList.size() != 0) {
            QString touchPointXml = translateTouchEvent(touchPointList);
            g_Link->requestTouchStatus(ANDROID_MIRROR, TouchEnd, touchPointXml);
        }
    }
}

QString MirrorLinkWidgetPrivate::translateTouchEvent(const QList<TouchEvent> &list)
{
    QString touchPointXml;
    touchPointXml.clear();
    QDomDocument domDocument;
    domDocument.appendChild(domDocument.createElement(QString("TouchEvent")));
    QDomElement root = domDocument.firstChildElement(QString("TouchEvent"));
    QDomElement touchPoint = domDocument.createElement(QString("TouchPoint"));
    root.appendChild(touchPoint);
    QDomElement positionState;
    QDomElement positionX;
    QDomElement positionY;
    QDomElement touchPointIndex;
    touchPointXml.clear();
    for (int i = 0; i < list.size(); ++i) {
        positionState = domDocument.createElement(QString("State"));
        positionState.appendChild(domDocument.createTextNode(QString::number(list.at(i).state)));
        touchPointIndex = domDocument.createElement(QString("Index:") + QString::number(i));
        touchPointIndex.appendChild(positionState);
        positionX = domDocument.createElement(QString("X"));
        positionX.appendChild(domDocument.createTextNode(QString::number(list.at(i).x)));
        touchPointIndex.appendChild(positionX);
        positionY = domDocument.createElement(QString("Y"));
        positionY.appendChild(domDocument.createTextNode(QString::number(list.at(i).y)));
        touchPointIndex.appendChild(positionY);
        touchPoint.appendChild(touchPointIndex);
    }
    touchPointXml = domDocument.toString();
    return touchPointXml;
}
