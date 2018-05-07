#include "CarlifeLinkWidget.h"
#include "AutoConnect.h"
#include "UserInterfaceUtility.h"
#include "BusinessLogic/BusinessLogicUtility.h"
#include "BusinessLogic/Setting.h"
#include "BusinessLogic/Bluetooth.h"
#include "UserInterfaceUtility.h"
#include <QMouseEvent>
#include <QPainter>
#include <QDomElement>

namespace SourceString {
static const QString Phone_OS_Limit_Unlock_Your_Phone_And_Open_Carlife(QObject::tr("Phone OS Limit,\n"
                                                                                   "Unlock Your Phone And Open Carlife..."));
}

class CarlifeLinkWidgetPrivate
{
    Q_DISABLE_COPY(CarlifeLinkWidgetPrivate)
public:
    explicit CarlifeLinkWidgetPrivate(CarlifeLinkWidget* parent);
    ~CarlifeLinkWidgetPrivate();
    void connectAllSlots();
    void initializeMessageBox();
    void onCarlifeLinkStatus(const Link_STATUS status);
    void touchBeginEventHandler(QEvent* event);
    void touchUpdateEventHandler(QEvent* event);
    void touchEndEventHandler(QEvent* event);
    QString translateTouchEvent(const QList<TouchEvent> &list);
    MessageBox* m_MessageBox;
private:
    CarlifeLinkWidget* m_Parent;
};

CarlifeLinkWidget::CarlifeLinkWidget(QWidget *parent)
    : QWidget(parent)
    , m_Private(new CarlifeLinkWidgetPrivate(this))
{
}

CarlifeLinkWidget::~CarlifeLinkWidget()
{
}

void CarlifeLinkWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(0, 0, 800, 480, this);
}

void CarlifeLinkWidget::mousePressEvent(QMouseEvent *event)
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
        g_Link->requestTouchStatus(CARLIFE, TouchBegin, touchPointXml);
    }
}

void CarlifeLinkWidget::mouseReleaseEvent(QMouseEvent *event)
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
        g_Link->requestTouchStatus(CARLIFE, TouchBegin, touchPointXml);
    }
}

void CarlifeLinkWidget::mouseMoveEvent(QMouseEvent *event)
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
        g_Link->requestTouchStatus(CARLIFE, TouchBegin, touchPointXml);
    }
}

bool CarlifeLinkWidget::event(QEvent *event)
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

void CarlifeLinkWidget::onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status)
{
    switch (destinationType) {
    case Widget::T_Home:
    case Widget::T_Back: {
        if (WidgetStatus::RequestShow == status) {
            if (isVisible()) {
                g_Link->requestLinkStatus(CARLIFE, LINK_EXITING);
            }
        } else if (WidgetStatus::Show == status) {
            setVisible(false);
        }
        break;
    }
    case Widget::T_Carlife: {
        if (WidgetStatus::RequestShow == status) {
            if (!isVisible()) {
                QFile addressfile(QString("/tmp/bluetoothaddress"));
                if (addressfile.open(QFile::WriteOnly)) {
                    QString localAddress = g_Bluetooth->localDeviceAddress();
                    addressfile.write(localAddress.toLocal8Bit().data(), localAddress.length());
                    addressfile.flush();
                    addressfile.close();
                }
                QFile pinfile(QString("/tmp/bluetoothpin"));
                if (pinfile.open(QFile::WriteOnly)) {
                    QString pincode = g_Bluetooth->pinCode();
                    pinfile.write(pincode.toLocal8Bit().data(), pincode.length());
                    pinfile.flush();
                    pinfile.close();
                }
                g_Link->requestLinkStatus(CARLIFE, LINK_STARTING);
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
                        && (CARLIFE == g_Link->getLinkType())) {
                    g_Link->requestLinkStatus(CARLIFE, LINK_EXITING);
                }
            }
            setVisible(false);
        }
        break;
    }
    }
}

void CarlifeLinkWidget::onLinkStatusChange(const Link_Type type, const Link_STATUS status)
{
    qDebug() << "CarlifeLinkWidget::onLinkStatusChange" << type << status;
    switch (type) {
    case CARLIFE: {
        m_Private->onCarlifeLinkStatus(status);
        break;
    }
    default: {
        break;
    }
    }
}

CarlifeLinkWidgetPrivate::CarlifeLinkWidgetPrivate(CarlifeLinkWidget* parent)
    : m_Parent(parent)
{
    m_MessageBox = NULL;
    connectAllSlots();
}

CarlifeLinkWidgetPrivate::~CarlifeLinkWidgetPrivate()
{
}

void CarlifeLinkWidgetPrivate::connectAllSlots()
{
    connectSignalAndSlotByNamesake(g_Widget, m_Parent, ARKRECEIVER(onWidgetTypeChange(const Widget::Type, const Widget::Type, const QString &)));
    connectSignalAndSlotByNamesake(g_Link, m_Parent, ARKRECEIVER(onLinkStatusChange(const int, const int)));
}

void CarlifeLinkWidgetPrivate::initializeMessageBox()
{
    if (NULL == m_MessageBox) {
        m_MessageBox = new MessageBox(m_Parent);
        m_MessageBox->setVisible(false);
        m_MessageBox->setText(SourceString::Phone_OS_Limit_Unlock_Your_Phone_And_Open_Carlife);
        m_MessageBox->setAutoHide(false);
        m_MessageBox->setFontPointSize(15 * g_Widget->widthScalabilityFactor());
        m_MessageBox->setBackgroundBmpPath(QString(":/Images/MessageBoxBackground.png"));
        g_Widget->geometryFit(0, 0, 800, 480, m_MessageBox);
    }
}

void CarlifeLinkWidgetPrivate::onCarlifeLinkStatus(const int status)
{
    qDebug() << "onCarlifeLinkStatus" << status;
    switch (status) {
    case LINK_STARTING: {
        if (NULL != m_MessageBox) {
            m_MessageBox->setVisible(false);
        }
        if (ACS_ReversingOff == g_Setting->getReversing()) {
            if (QFile::exists(QString("/tmp/video"))) {
                QFile::remove(QString("/tmp/video"));
            }
        }
        break;
    }
    case LINK_REMOVED:
    case LINK_EXITING: {
        g_Setting->setLauncherTVOUT(true);
        break;
    }
    case LINK_CALL_PHONE: {
        g_Widget->acquireWidgetType(Widget::T_Carlife);
        break;
    }
    case LINK_CALL_PHONE_EXITED: {
        g_Widget->releaseWidgetType(Widget::T_Carlife);
        break;
    }
    case LINK_NODATA: {
        initializeMessageBox();
        m_MessageBox->setVisible(true);
        break;
    }
    case LINK_VIDEOREADY: {
        if (NULL != m_MessageBox) {
            m_MessageBox->setVisible(false);
        }
        break;
    }
    case LINK_SUCCESS: {
        g_Setting->setLauncherTVOUT(false);
        if (ACS_ReversingOff == g_Setting->getReversing()) {
            if (QFile::exists(QString("/tmp/video"))) {
                QFile::remove(QString("/tmp/video"));
            }
        }
        g_Widget->setWidgetType(Widget::T_Carlife, Widget::T_Link, WidgetStatus::Show);
        break;
    }
    default: {
        break;
    }
    }
}

void CarlifeLinkWidgetPrivate::touchBeginEventHandler(QEvent *event)
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
            g_Link->requestTouchStatus(CARLIFE, TouchBegin, touchPointXml);
        }
    }
}

void CarlifeLinkWidgetPrivate::touchUpdateEventHandler(QEvent *event)
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
            g_Link->requestTouchStatus(CARLIFE, TouchUpdate, touchPointXml);
        }
    }
}

void CarlifeLinkWidgetPrivate::touchEndEventHandler(QEvent *event)
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
            g_Link->requestTouchStatus(CARLIFE, TouchEnd, touchPointXml);
        }
    }
}

QString CarlifeLinkWidgetPrivate::translateTouchEvent(const QList<TouchEvent> &list)
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
