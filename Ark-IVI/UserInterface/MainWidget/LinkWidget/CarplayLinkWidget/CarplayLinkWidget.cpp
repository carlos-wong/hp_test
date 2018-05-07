#include "CarplayLinkWidget.h"
#include "AutoConnect.h"
#include "UserInterfaceUtility.h"
#include "BusinessLogic/BusinessLogicUtility.h"
#include "BusinessLogic/Setting.h"
#include "MessageBox.h"
#include <QMouseEvent>
#include <QPainter>
#include <QDomElement>

class CarplayLinkWidgetPrivate
{
    Q_DISABLE_COPY(CarplayLinkWidgetPrivate)
public:
    explicit CarplayLinkWidgetPrivate(CarplayLinkWidget* parent);
    ~CarplayLinkWidgetPrivate();
    void connectAllSlots();
    void onCarplayLinkStatus(const Link_STATUS status);
    void touchBeginEventHandler(QEvent* event);
    void touchUpdateEventHandler(QEvent* event);
    void touchEndEventHandler(QEvent* event);
    QString translateTouchEvent(const QList<TouchEvent> &list);
private:
    CarplayLinkWidget* m_Parent;
};

CarplayLinkWidget::CarplayLinkWidget(QWidget *parent)
    : QWidget(parent)
    , m_Private(new CarplayLinkWidgetPrivate(this))
{
}

CarplayLinkWidget::~CarplayLinkWidget()
{
}

void CarplayLinkWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(0, 0, 800, 480, this);
}

void CarplayLinkWidget::mousePressEvent(QMouseEvent *event)
{
    qDebug() << __FUNCTION__ << event->pos() << qgetenv("QWS_ARK_MT_DEVICE").isEmpty();
    if (qgetenv("QWS_ARK_MT_DEVICE").isEmpty()) {
        QList<TouchEvent> touchPointList;
        touchPointList.clear();
        TouchEvent touchEvent;
        touchEvent.state = TouchEvent::TouchPointPressed;
        touchEvent.x = event->pos().x();
        touchEvent.y = event->pos().y();
        touchPointList.append(touchEvent);
        QString touchPointXml = m_Private->translateTouchEvent(touchPointList);
        g_Link->requestTouchStatus(CARPLAY, TouchBegin, touchPointXml);
    }
}

void CarplayLinkWidget::mouseReleaseEvent(QMouseEvent *event)
{
    //        qDebug() << __FUNCTION__ << event->pos() << qgetenv("QWS_ARK_MT_DEVICE").isEmpty();
    if (qgetenv("QWS_ARK_MT_DEVICE").isEmpty()) {
        QList<TouchEvent> touchPointList;
        touchPointList.clear();
        TouchEvent touchEvent;
        touchEvent.state = TouchEvent::TouchPointReleased;
        touchEvent.x = event->pos().x();
        touchEvent.y = event->pos().y();
        touchPointList.append(touchEvent);
        QString touchPointXml = m_Private->translateTouchEvent(touchPointList);
        g_Link->requestTouchStatus(CARPLAY, TouchBegin, touchPointXml);
    }
}

void CarplayLinkWidget::mouseMoveEvent(QMouseEvent *event)
{
    //    qDebug() << __FUNCTION__ << event->pos() << qgetenv("QWS_ARK_MT_DEVICE").isEmpty();
    if (qgetenv("QWS_ARK_MT_DEVICE").isEmpty()) {
        QList<TouchEvent> touchPointList;
        touchPointList.clear();
        TouchEvent touchEvent;
        touchEvent.state = TouchEvent::TouchPointMoved;
        touchEvent.x = event->pos().x();
        touchEvent.y = event->pos().y();
        touchPointList.append(touchEvent);
        QString touchPointXml = m_Private->translateTouchEvent(touchPointList);
        g_Link->requestTouchStatus(CARPLAY, TouchBegin, touchPointXml);
    }
}

bool CarplayLinkWidget::event(QEvent *event)
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

void CarplayLinkWidget::onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status)
{
    switch (destinationType) {
    case Widget::T_Home:
    case Widget::T_Back: {
        if (WidgetStatus::RequestShow == status) {
            if (isVisible()) {
                g_Link->requestLinkStatus(CARPLAY, LINK_EXITING);
            }
        } else if (WidgetStatus::Show == status) {
            setVisible(false);
        }
        break;
    }
    case Widget::T_Carplay: {
        if (WidgetStatus::RequestShow == status) {
            if (!isVisible()) {
                g_Link->requestLinkStatus(CARPLAY, LINK_STARTING);
            }
        } else if (WidgetStatus::Show == status) {
            raise();
            if (!qgetenv("QWS_ARK_MT_DEVICE").isEmpty()) {
                setAttribute(Qt::WA_AcceptTouchEvents, true);
            }
            setVisible(true);
        }
        break;
    }
    default: {
        if (WidgetStatus::Show == status) {
            if (!isHidden()) {
                if ((LINK_SUCCESS == g_Link->getLinkStatus())
                        && (CARPLAY == g_Link->getLinkType())) {
                    g_Link->requestLinkStatus(CARPLAY, LINK_EXITING);
                }
            }
            setVisible(false);
        }
        break;
    }
    }
}

void CarplayLinkWidget::onLinkStatusChange(const Link_Type type, const Link_STATUS status)
{
    qDebug() << "CarplayLinkWidget::onLinkStatusChange" << type << status;
    switch (type) {
    case CARPLAY: {
        m_Private->onCarplayLinkStatus(status);
        break;
    }
    default: {
        break;
    }
    }
}

CarplayLinkWidgetPrivate::CarplayLinkWidgetPrivate(CarplayLinkWidget* parent)
    : m_Parent(parent)

{
    connectAllSlots();
}

CarplayLinkWidgetPrivate::~CarplayLinkWidgetPrivate()
{
}

void CarplayLinkWidgetPrivate::connectAllSlots()
{
    connectSignalAndSlotByNamesake(g_Widget, m_Parent, ARKRECEIVER(onWidgetTypeChange(const Widget::Type, const Widget::Type, const QString &)));
    connectSignalAndSlotByNamesake(g_Link, m_Parent, ARKRECEIVER(onLinkStatusChange(const int, const int)));
}

void CarplayLinkWidgetPrivate::onCarplayLinkStatus(const int status)
{
    qDebug() << "onCarplayLinkStatus" << status;
    switch (status) {
    case LINK_STARTING: {
        if (ACS_ReversingOff == g_Setting->getReversing()) {
            if (QFile::exists(QString("/tmp/video"))) {
                QFile::remove(QString("/tmp/video"));
            }
        }
        break;
    }
    case LINK_DISCONNECTED: {
        break;
    }
    case LINK_REMOVED:
    case LINK_EXITING: {
        g_Setting->setLauncherTVOUT(true);
        break;
    }
    case LINK_EXITED: {
        break;
    }
    case LINK_SUCCESS: {
        g_Setting->setLauncherTVOUT(false);
        if (ACS_ReversingOff == g_Setting->getReversing()) {
            if (QFile::exists(QString("/tmp/video"))) {
                QFile::remove(QString("/tmp/video"));
            }
        }
        g_Widget->setWidgetType(Widget::T_Carplay, Widget::T_Link, WidgetStatus::Show);
        break;
    }
    case LINK_CALL_PHONE: {
        g_Widget->acquireWidgetType(Widget::T_Carplay);
        break;
    }
    case LINK_CALL_PHONE_EXITED: {
        g_Widget->releaseWidgetType(Widget::T_Carplay);
        break;
    }
    default: {
        break;
    }
    }
}

void CarplayLinkWidgetPrivate::touchBeginEventHandler(QEvent *event)
{
    QTouchEvent* ptr = static_cast<QTouchEvent*>(event);
    if (NULL != ptr) {
        qDebug() << __FUNCTION__ << ptr->touchPointStates();
        QList<QTouchEvent::TouchPoint> touchPoints = ptr->touchPoints();
        QList<TouchEvent> touchPointList;
        touchPointList.clear();
        for (int i = 0; i < touchPoints.size(); ++i) {
            //            qDebug() << i << touchPoints.at(i).state() << touchPoints.at(i).pos().toPoint();
            TouchEvent touchEvent;
            touchEvent.state = static_cast<TouchEvent::TouchPointState>(touchPoints.at(i).state());
            touchEvent.x = touchPoints.at(i).pos().toPoint().x();
            touchEvent.y = touchPoints.at(i).pos().toPoint().y();
            touchPointList.append(touchEvent);
        }
        if (touchPointList.size() != 0) {
            QString touchPointXml = translateTouchEvent(touchPointList);
            g_Link->requestTouchStatus(CARPLAY, TouchBegin, touchPointXml);
        }
    }
}

void CarplayLinkWidgetPrivate::touchUpdateEventHandler(QEvent *event)
{
    QTouchEvent* ptr = static_cast<QTouchEvent*>(event);
    if (NULL != ptr) {
        //        qDebug() << __FUNCTION__ << ptr->touchPointStates();
        QList<QTouchEvent::TouchPoint> touchPoints = ptr->touchPoints();
        QList<TouchEvent> touchPointList;
        touchPointList.clear();
        for (int i = 0; i < touchPoints.size(); ++i) {
            //            qDebug() << i << touchPoints.at(i).state() << touchPoints.at(i).pos();
            TouchEvent touchEvent;
            touchEvent.state = static_cast<TouchEvent::TouchPointState>(touchPoints.at(i).state());
            touchEvent.x = touchPoints.at(i).pos().toPoint().x();
            touchEvent.y = touchPoints.at(i).pos().toPoint().y();
            touchPointList.append(touchEvent);
        }
        if (touchPointList.size() != 0) {
            QString touchPointXml = translateTouchEvent(touchPointList);
            g_Link->requestTouchStatus(CARPLAY, TouchUpdate, touchPointXml);
        }
    }
}

void CarplayLinkWidgetPrivate::touchEndEventHandler(QEvent *event)
{
    QTouchEvent* ptr = static_cast<QTouchEvent*>(event);
    if (NULL != ptr) {
        //        qDebug() << __FUNCTION__ << ptr->touchPointStates();
        QList<QTouchEvent::TouchPoint> touchPoints = ptr->touchPoints();
        QList<TouchEvent> touchPointList;
        touchPointList.clear();
        for (int i = 0; i < touchPoints.size(); ++i) {
            //            qDebug() << i << touchPoints.at(i).state() << touchPoints.at(i).pos();
            TouchEvent touchEvent;
            touchEvent.state = static_cast<TouchEvent::TouchPointState>(touchPoints.at(i).state());
            touchEvent.x = touchPoints.at(i).pos().toPoint().x();
            touchEvent.y = touchPoints.at(i).pos().toPoint().y();
            touchPointList.append(touchEvent);
        }
        if (touchPointList.size() != 0) {
            QString touchPointXml = translateTouchEvent(touchPointList);
            g_Link->requestTouchStatus(CARPLAY, TouchEnd, touchPointXml);
        }
    }
}

QString CarplayLinkWidgetPrivate::translateTouchEvent(const QList<TouchEvent> &list)
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
