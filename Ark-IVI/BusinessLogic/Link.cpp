#include "Link.h"
#include "Widget.h"
#include "CarlifeLinkProxy.h"
#include "CarplayLinkProxy.h"
#include "AutoLinkProxy.h"
#include "Utility.h"
#include "AutoConnect.h"
#include "Setting.h"
#include "Radio.h"
#include "BusinessLogic/BusinessLogicUtility.h"
#include "UserInterfaceUtility.h"
#include "RunnableThread.h"
#include "BusinessLogic/Audio.h"
#include <unistd.h>

#define ArkMicroCarlifeService          QString("com.arkmicro.carlife")
#define ArkMicroCarlifePath             QString("/com/arkmicro/carlife")
#define ArkMicroCarlifeInterface        QString("Local.DbusServer.Carlife")
#define ArkMicroCarplayService          QString("com.arkmicro.carplay")
#define ArkMicroCarplayPath             QString("/com/arkmicro/carplay")
#define ArkMicroCarplayInterface        QString("Local.DbusServer.Carplay")
#define ArkMicroAutoService             QString("com.arkmicro.auto")
#define ArkMicroAutoPath                QString("/com/arkmicro/auto")
#define ArkMicroAutoInterface           QString("Local.DbusServer.Auto")

class LinkPrivate
{
public:
    explicit LinkPrivate(Link *parent);
    ~LinkPrivate();
    void initialize();
    void connectAllSlots();
    void requestCarlifeHandler(const Link_STATUS status);
    void carlifeSourceHandler(const Link_STATUS status);
    void requestAutoHandler(const Link_STATUS status);
    void autoSourceHandler(const Link_STATUS status);
    void requestCarplayHandler(const Link_STATUS status);
    void carplaySourceHandler(const Link_STATUS status);
    Local::DbusServer::Carlife* m_CarlifeLinkProxy;
    Local::DbusServer::Auto* m_AutoLinkProxy;
    Local::DbusServer::Carplay* m_CarplayLinkProxy;
    Link_Type m_Link_Type;
    Link_STATUS m_Link_STATUS;
private:
    Link* m_Parent;
};

void Link::startCarlife()
{
    initializePrivate();
    static bool flag = true;
    if (flag) {
        flag = false;
        QDBusConnection::sessionBus().interface()->startService(ArkMicroCarlifeService);
    }
}

void Link::startCarplay()
{
    initializePrivate();
    static bool flag = true;
    if (flag) {
        flag = false;
        QDBusConnection::sessionBus().interface()->startService(ArkMicroCarplayService);
    }
}

void Link::startAuto()
{
    initializePrivate();
    static bool flag = true;
    if (flag) {
        flag = false;
        QDBusConnection::sessionBus().interface()->startService(ArkMicroAutoService);
    }
}

void Link::reset()
{
}

void Link::requestLinkStatus(const Link_Type type, const Link_STATUS status)
{
    qDebug() << __PRETTY_FUNCTION__ << __LINE__ << type << status;
    initializePrivate();
    switch (type) {
    case CARLIFE: {
        m_Private->requestCarlifeHandler(status);
        break;
    }
    case AUTO: {
        m_Private->requestAutoHandler(status);
        break;
    }
    case CARPLAY: {
        m_Private->requestCarplayHandler(status);
        break;
    }
    default: {
        qDebug() << "unsupport command:" << type << status;
    }
    }
}

void Link::requestTouchStatus(const Link_Type deviceType, const TouchType touchType, const QString &touchPointXml)
{
    //    qDebug() << "Link::requestTouchStatus" << deviceType << touchType << touchPointXml;
    initializePrivate();
    switch (deviceType) {
    case CARLIFE: {
        QDBusPendingReply<> reply = m_Private->m_CarlifeLinkProxy->requestTouchStatus(deviceType, touchType, touchPointXml);
        //        reply.waitForFinished();
        //        qDebug() << "requestTouchStatus" << reply.isFinished();
        //        if (reply.isError()) {
        //            qDebug() << "method call requestTouchStatus failed" << reply.error();
        //        }
        break;
    }
    case AUTO: {
        if (NULL != m_Private->m_AutoLinkProxy) {
            QDBusPendingReply<> reply = m_Private->m_AutoLinkProxy->requestTouchStatus(deviceType, touchType, touchPointXml);
            reply.waitForFinished();
            qDebug() << "requestTouchStatus" << reply.isFinished();
            if (reply.isError()) {
                qDebug() << "method call requestTouchStatus failed" << reply.error();
            }
        }
        break;
    }
    case CARPLAY: {
        QDBusPendingReply<> reply = m_Private->m_CarplayLinkProxy->requestTouchStatus(deviceType, touchType, touchPointXml);
        //        reply.waitForFinished();
        //        qDebug() << "requestTouchStatus" << reply.isFinished();
        //        if (reply.isError()) {
        //            qDebug() << "method call requestTouchStatus failed" << reply.error();
        //        }
        break;
    }
    default: {
        break;
    }
    }
}

void Link::requestWheelStatus(const int deviceType, const int select_pressed, const int wheelStep)
{
    //    qDebug() << "Link::requestWheelStatus" << deviceType << touchType << touchPointXml;
    initializePrivate();
    switch (deviceType) {
    case CARLIFE: {
        if (NULL != m_Private->m_CarlifeLinkProxy) {
            QDBusPendingReply<> reply = m_Private->m_CarlifeLinkProxy->requestWheelStatus(deviceType, select_pressed, wheelStep);
            //        reply.waitForFinished();
            //        qDebug() << "requestWheelStatus" << reply.isFinished();
            //        if (reply.isError()) {
            //            qDebug() << "method call requestWheelStatus failed" << reply.error();
            //        }
        }
        break;
    }
//    case AUTO: {
//        if (NULL != m_Private->m_AutoLinkProxy) {
//            QDBusPendingReply<> reply = m_Private->m_AutoLinkProxy->requestTouchStatus(deviceType, touchType, touchPointXml);
//            reply.waitForFinished();
//            qDebug() << "requestWheelStatus" << reply.isFinished();
//            if (reply.isError()) {
//                qDebug() << "method call requestWheelStatus failed" << reply.error();
//            }
//        }
//        break;
//    }
    case CARPLAY: {
        QDBusPendingReply<> reply = m_Private->m_CarlifeLinkProxy->requestWheelStatus(deviceType, select_pressed, wheelStep);
        //        reply.waitForFinished();
        //        qDebug() << "requestWheelStatus" << reply.isFinished();
        //        if (reply.isError()) {
        //            qDebug() << "method call requestWheelStatus failed" << reply.error();
        //        }
        break;
    }
    default: {
        break;
    }
    }
}

void Link::requestKeyValue(const Link_Type deviceType, const KeyValue type)
{
    switch (deviceType) {
    case CARPLAY: {
        initializePrivate();
        QDBusPendingReply<> reply = m_Private->m_CarplayLinkProxy->requestKeyValue(type);
        reply.waitForFinished();
        if (reply.isError()) {
            qDebug() << "method call requestKeyValue failed" << reply.error();
        }
        break;
    }
    case CARLIFE: {
        initializePrivate();
        QDBusPendingReply<> reply = m_Private->m_CarlifeLinkProxy->requestKeyValue(type);
        reply.waitForFinished();
        if (reply.isError()) {
            qDebug() << "method call requestKeyValue failed" << reply.error();
        }
        break;
    }
    default: {
        break;
    }
    }
}
//0 idle
//1 ready
//2 connected
void Link::reqeustBTInfo(const Link_Type type, const int status)
{
    initializePrivate();
    switch (type) {
    case CARLIFE: {
        initializePrivate();
        QDBusPendingReply<> reply = m_Private->m_CarlifeLinkProxy->reqeustBTInfo(status);
        reply.waitForFinished();
        if (reply.isError()) {
            qDebug() << "method call requestKeyValue failed" << reply.error();
        }
    }
    default: {
        break;
    }
    }
}

int Link::getLinkType()
{
    initializePrivate();
    qDebug() << __PRETTY_FUNCTION__ << m_Private->m_Link_Type;
    return m_Private->m_Link_Type;
}

int Link::getLinkStatus()
{
    initializePrivate();
    qDebug() << __PRETTY_FUNCTION__ << m_Private->m_Link_STATUS;
    return m_Private->m_Link_STATUS;
}

void Link::onServiceUnregistered(const QString &service)
{
    qDebug() << "Link::onServiceUnregistered" << service;
    if (ArkMicroCarlifeService == service) {
        onLinkStatusChange(CARLIFE, LINK_REMOVED);
        g_DbusService->startService(service);
    } else if (ArkMicroAutoService == service) {
        onLinkStatusChange(AUTO, LINK_REMOVED);
        g_DbusService->startService(service);
    } else if (ArkMicroCarplayService == service) {
        onLinkStatusChange(CARPLAY, LINK_REMOVED);
        g_DbusService->startService(service);
    }
}

void Link::linkStatusChange(const int type, const int status)
{
    qDebug() << "Link::linkStatusChange" << type << status;
    switch (type) {
    case CARLIFE: {
        m_Private->carlifeSourceHandler(status);
        break;
    }
    case AUTO: {
        m_Private->autoSourceHandler(status);
        break;
    }
    case CARPLAY: {
        m_Private->carplaySourceHandler(status);
        break;
    }
    default: {
        break;
    }
    }
}

void Link::onHolderChange(const AudioSource oldHolder, const AudioSource newHolder)
{
    qDebug() << __PRETTY_FUNCTION__ << __LINE__ << oldHolder << newHolder;
    switch (newHolder) {
    case AS_Video: {
        if (-1 != m_Private->m_Link_Type) {
            requestLinkStatus(m_Private->m_Link_Type, LINK_EXITED);
        }
        break;
    }
    default: {
        switch (oldHolder) {
        case AS_Aux: {
            switch (newHolder) {
            case AS_CarplayMusic:
            case AS_AutoMusic:
            case AS_CarlifeMusic: {
                g_Audio->requestAudioSource(newHolder);
                break;
            }
            default: {
                break;
            }
            }
            break;
        }
        case AS_CarplayMusic: {
            if (AS_CarplayPhone != newHolder) {
                if (-1 != m_Private->m_Link_Type) {
                    qDebug() << __PRETTY_FUNCTION__ << __LINE__ << m_Private->m_Link_Type << newHolder;
                    if (AS_Aux != newHolder) {
                        requestLinkStatus(CARPLAY, LINK_EXITED);
                    }
                }
            }
            break;
        }
        case AS_AutoMusic: {
            if (AS_AutoPhone != newHolder) {
                if (-1 != m_Private->m_Link_Type) {
                    if (AS_Aux != newHolder) {
                        requestLinkStatus(AUTO, LINK_EXITED);
                    }
                }
            }
            break;
        }
        case AS_AutoPhone: {
            if (AS_AutoMusic == newHolder) {
                if (AS_AutoPhone != g_Audio->getAudioSource()) {
                    g_Audio->requestAudioSource(AS_AutoMusic);
                }
            } else {
                if (-1 != m_Private->m_Link_Type) {
                    requestLinkStatus(AUTO, LINK_EXITED);
                }
            }
            break;
        }
        case AS_CarlifeMusic: {
            if (AS_CarlifePhone != newHolder) {
                if (-1 != m_Private->m_Link_Type) {
                    if (AS_Aux != newHolder) {
                        requestLinkStatus(CARLIFE, LINK_EXITED);
                    }
                }
            }
            break;
        }
        case AS_CarlifePhone: {
            if (AS_CarlifeMusic == newHolder) {
                if (AS_CarlifePhone != g_Audio->getAudioSource()) {
                    g_Audio->requestAudioSource(AS_CarlifeMusic);
                }
            } else {
                if (-1 != m_Private->m_Link_Type) {
                    requestLinkStatus(CARLIFE, LINK_EXITED);
                }
            }
            break;
        }
        default: {
            break;
        }
        }
        break;
    }
    }
}

Link::Link(QObject *parent)
    : QObject(parent)
    , m_Private(NULL)
{
}

Link::~Link()
{
}

void Link::initializePrivate()
{
    if (NULL == m_Private) {
        m_Private.reset(new LinkPrivate(this));
    }
}

LinkPrivate::LinkPrivate(Link *parent)
    : m_Parent(parent)
{
    m_CarlifeLinkProxy = NULL;
    m_AutoLinkProxy = NULL;
    m_CarplayLinkProxy = NULL;
    m_Link_Type = -1;
    m_Link_STATUS = LINK_EXITED;
    initialize();
    connectAllSlots();
}

LinkPrivate::~LinkPrivate()
{
}

void LinkPrivate::initialize()
{
    memallocload();
    driverload();
    CarLinkType type = SettingPersistent::getCarLinkType();
    if (CLT_CarlifeCarplay == type) {
        if (NULL == m_CarlifeLinkProxy) {
            m_CarlifeLinkProxy = new Local::DbusServer::Carlife(ArkMicroCarlifeService,
                                                                ArkMicroCarlifePath,
                                                                QDBusConnection::sessionBus(),
                                                                m_Parent);
        }
    } else {
        if (NULL == m_AutoLinkProxy) {
            m_AutoLinkProxy = new Local::DbusServer::Auto(ArkMicroAutoService,
                                                          ArkMicroAutoPath,
                                                          QDBusConnection::sessionBus(),
                                                          m_Parent);
        }
    }
    if (NULL == m_CarplayLinkProxy) {
        m_CarplayLinkProxy = new Local::DbusServer::Carplay(ArkMicroCarplayService,
                                                            ArkMicroCarplayPath,
                                                            QDBusConnection::sessionBus(),
                                                            m_Parent);
    }
    if (CLT_CarlifeCarplay == type) {
        g_DbusService->addWatchedService(ArkMicroAutoService);
    } else {
        g_DbusService->addWatchedService(ArkMicroCarlifeService);
    }
    g_DbusService->addWatchedService(ArkMicroCarplayService);
    connectSignalAndSlotByNamesake(g_DbusService, m_Parent, ARKRECEIVER(onServiceUnregistered(const QString &)));
}

void LinkPrivate::connectAllSlots()
{
    connectSignalAndSlotByNamesake(g_DbusService, m_Parent, ARKRECEIVER(onServiceUnregistered(const QString &)));
    connectSignalAndSlotByNamesake(g_Audio, m_Parent, ARKRECEIVER(onHolderChange(const int, const int)));
    if (NULL != m_CarlifeLinkProxy) {
        Qt::ConnectionType type = static_cast<Qt::ConnectionType>(Qt::AutoConnection | Qt::UniqueConnection);
        QObject::connect(m_CarlifeLinkProxy, ARKSENDER(onLinkStatusChange(int, int)),
                         m_Parent,          ARKRECEIVER(linkStatusChange(int, int)),
                         type);
    }
    if (NULL != m_AutoLinkProxy) {
        Qt::ConnectionType type = static_cast<Qt::ConnectionType>(Qt::AutoConnection | Qt::UniqueConnection);
        QObject::connect(m_AutoLinkProxy, ARKSENDER(onLinkStatusChange(int, int)),
                         m_Parent,        ARKRECEIVER(linkStatusChange(int, int)),
                         type);
    }
    if (NULL != m_CarplayLinkProxy) {
        Qt::ConnectionType type = static_cast<Qt::ConnectionType>(Qt::AutoConnection | Qt::UniqueConnection);
        QObject::connect(m_CarplayLinkProxy, ARKSENDER(onLinkStatusChange(int, int)),
                         m_Parent,           ARKRECEIVER(linkStatusChange(int, int)),
                         type);
    }
}

void LinkPrivate::requestCarlifeHandler(const Link_STATUS status)
{
    qDebug() << "requestCarlifeHandler" << status;
    if (NULL != m_CarlifeLinkProxy) {
        QDBusPendingReply<> reply = m_CarlifeLinkProxy->requestLinkStatus(CARLIFE, status);
        reply.waitForFinished();
        if (reply.isError()) {
            qDebug() << "method call" << __PRETTY_FUNCTION__ << " failed" << reply.error();
        }
    }
}

void LinkPrivate::requestCarplayHandler(const Link_STATUS status)
{
    qDebug() << "requestCarplayHandler" << status;
    QDBusPendingReply<> reply = m_CarplayLinkProxy->requestLinkStatus(CARPLAY, status);
    reply.waitForFinished();
    if (reply.isError()) {
        qDebug() << "method call" << __PRETTY_FUNCTION__ << " failed" << reply.error();
    }
}

void LinkPrivate::carlifeSourceHandler(const Link_STATUS status)
{
    qDebug() << " LinkPrivate::carlifeSourceHandler" << status;
    switch (status) {
    case LINK_STARTING: {
        m_Link_Type = CARLIFE;
        if ((LINK_EXITING != m_Link_STATUS)
                || (LINK_EXITED == m_Link_STATUS)) {
            if (AS_CarlifePhone != g_Audio->getAudioSource()) {
                g_Audio->requestAudioSource(AS_CarlifeMusic);
            }
        }
        break;
    }
    case LINK_EXITING: {
        if (AS_CarlifePhone == g_Audio->getAudioSource()) {
            m_Parent->requestLinkStatus(CARLIFE, LINK_STARTING);
            return;
        }
        m_Link_Type = CARLIFE;
        m_Link_STATUS = LINK_EXITING;
        break;
    }
    case LINK_SUCCESS: {
        if (LINK_EXITING == m_Link_STATUS) {
            if (AS_CarlifePhone != g_Audio->getAudioSource()) {
                g_Audio->requestAudioSource(AS_CarlifeMusic);
            }
        }
        m_Link_Type = CARLIFE;
        m_Link_STATUS = LINK_SUCCESS;
        break;
    }
    case LINK_REMOVED: {
        if (AS_CarlifePhone == g_Audio->getAudioSource()) {
            carlifeSourceHandler(LINK_CALL_PHONE_EXITED);
            m_Parent->onLinkStatusChange(CARLIFE, LINK_CALL_PHONE_EXITED);
        }
        m_Link_Type = -1;
        m_Link_STATUS = LINK_EXITED;
        if (AS_CarlifeMusic == g_Audio->getMultimediaSource()) {
            g_Radio->initialize();
        } else {
            qDebug() << __PRETTY_FUNCTION__ << "request radio1 failed!";
        }
        break;
    }
    case LINK_EXITED: {
        m_Link_Type = CARLIFE;
        m_Link_STATUS = LINK_EXITED;
        break;
    }
    case LINK_CALL_PHONE: {
        if (DST_Off == g_Setting->getDisplayScreenType()) {
            usleep(66666);
            system("echo 30 > /sys/devices/platform/pwm-backlight/backlight/pwm-backlight/brightness");
        }
        g_Audio->requestAudioSource(AS_CarlifePhone);
        break;
    }
    case LINK_CALL_PHONE_EXITED: {
        if (DST_Off == g_Setting->getDisplayScreenType()) {
            if (ACS_ReversingOff == g_Setting->getReversing()) {
                system("echo 100 > /sys/devices/platform/pwm-backlight/backlight/pwm-backlight/brightness");
            }
        }
        if (AS_CarlifePhone == g_Audio->getAudioSource()) {
            g_Audio->releaseAudioSource(AS_CarlifePhone);
        }
        break;
    }
    default: {
        break;
    }
    }
    m_Parent->onLinkStatusChange(CARLIFE, status);
}

void LinkPrivate::requestAutoHandler(const int status)
{
    qDebug() << "requestAutoHandler" << status;
    if (NULL != m_AutoLinkProxy) {
        QDBusPendingReply<> reply = m_AutoLinkProxy->requestLinkStatus(AUTO, status);
        reply.waitForFinished();
        if (reply.isError()) {
            qDebug() << "method call" << __PRETTY_FUNCTION__ << " failed" << reply.error();
        }
    }
}

void LinkPrivate::autoSourceHandler(const int status)
{
    qDebug() << " LinkPrivate::carlifeSourceHandler" << status;
    switch (status) {
    case LINK_STARTING: {
        m_Link_Type = AUTO;
        if ((LINK_EXITING != m_Link_STATUS)
                || (LINK_EXITED == m_Link_STATUS)) {
            if (AS_AutoMusic != g_Audio->getAudioSource()) {
                g_Audio->requestAudioSource(AS_AutoMusic);
            }
        }
        break;
    }
    case LINK_EXITING: {
        if (AS_AutoPhone == g_Audio->getAudioSource()) {
            m_Parent->requestLinkStatus(AUTO, LINK_STARTING);
            return;
        }
        m_Link_Type = AUTO;
        m_Link_STATUS = LINK_EXITING;
        break;
    }
    case LINK_SUCCESS: {
        if (LINK_EXITING == m_Link_STATUS) {
            if (AS_AutoPhone != g_Audio->getAudioSource()) {
                g_Audio->requestAudioSource(AS_AutoMusic);
            }
        }
        m_Link_Type = AUTO;
        m_Link_STATUS = LINK_SUCCESS;
        break;
    }
    case LINK_REMOVED: {
        if (AS_AutoPhone == g_Audio->getAudioSource()) {
            autoSourceHandler(LINK_CALL_PHONE_EXITED);
            m_Parent->onLinkStatusChange(AUTO, LINK_CALL_PHONE_EXITED);
        }
        m_Link_Type = -1;
        m_Link_STATUS = LINK_EXITED;
        if (AS_AutoMusic == g_Audio->getMultimediaSource()) {
            g_Radio->initialize();
        }
        break;
    }
    case LINK_EXITED: {
        m_Link_Type = AUTO;
        m_Link_STATUS = LINK_EXITED;
        break;
    }
    case LINK_CALL_PHONE: {
        if (DST_Off == g_Setting->getDisplayScreenType()) {
            usleep(66666);
            system("echo 30 > /sys/devices/platform/pwm-backlight/backlight/pwm-backlight/brightness");
        }
        if (AS_AutoPhone != g_Audio->getAudioSource()){
            g_Audio->requestAudioSource(AS_AutoPhone);
        }
        break;
    }
    case LINK_CALL_PHONE_EXITED: {
        if (DST_Off == g_Setting->getDisplayScreenType()) {
            if (ACS_ReversingOff == g_Setting->getReversing()) {
                system("echo 100 > /sys/devices/platform/pwm-backlight/backlight/pwm-backlight/brightness");
            }
        }
        if (AS_AutoPhone == g_Audio->getAudioSource()) {
            g_Audio->releaseAudioSource(AS_AutoPhone);
        }
        break;
    }
    default: {
        break;
    }
    }
    m_Parent->onLinkStatusChange(AUTO, status);
}

void LinkPrivate::carplaySourceHandler(const Link_STATUS status)
{
    qDebug() << " LinkPrivate::carplaySourceHandler" << status;
    //    m_CarplayFilter = false;
    switch (status) {
    case LINK_STARTING: {
        m_Link_Type = CARPLAY;
        if ((LINK_EXITING != m_Link_STATUS)
                || (LINK_EXITED == m_Link_STATUS)) {
            if (AS_CarplayPhone != g_Audio->getAudioSource()) {
                g_Audio->requestAudioSource(AS_CarplayMusic);
            }
        }
        break;
    }
    case LINK_EXITING: {
        if (AS_CarplayPhone == g_Audio->getAudioSource()) {
            m_Parent->requestLinkStatus(CARPLAY, LINK_STARTING);
            return;
        }
        m_Link_Type = CARPLAY;
        m_Link_STATUS = LINK_EXITING;
        break;
    }
    case LINK_SUCCESS: {
        m_Link_Type = CARPLAY;
        if (LINK_EXITING == m_Link_STATUS) {
            if (AS_CarplayPhone != g_Audio->getAudioSource()) {
                g_Audio->requestAudioSource(AS_CarplayMusic);
            }
        }
        m_Link_STATUS = LINK_SUCCESS;
        break;
    }
    case LINK_REMOVED: {
        m_Link_Type = -1;
        m_Link_STATUS = LINK_REMOVED;
        qDebug() << __PRETTY_FUNCTION__ << __LINE__;
        if (AS_CarplayPhone == g_Audio->getAudioSource()) {
            qDebug() << __PRETTY_FUNCTION__ << __LINE__;
            carplaySourceHandler(LINK_CALL_PHONE_EXITED);
            m_Parent->onLinkStatusChange(CARPLAY, LINK_CALL_PHONE_EXITED);
        }
        if (AS_CarplayMusic == g_Audio->getMultimediaSource()) {
            g_Radio->initialize();
        } else {
            qDebug() << __PRETTY_FUNCTION__ << "request radio failed!";
        }
        break;
    }
    case LINK_EXITED: {
        m_Link_Type = CARPLAY;
        m_Link_STATUS = LINK_EXITED;
        break;
    }
    case LINK_CALL_PHONE: {
        if (DST_Off == g_Setting->getDisplayScreenType()) {
            usleep(66666);
            system("echo 30 > /sys/devices/platform/pwm-backlight/backlight/pwm-backlight/brightness");
        }
        g_Audio->requestAudioSource(AS_CarplayPhone);
        break;
    }
    case LINK_CALL_PHONE_EXITED: {
        if (DST_Off == g_Setting->getDisplayScreenType()) {
            if (ACS_ReversingOff == g_Setting->getReversing()) {
                system("echo 100 > /sys/devices/platform/pwm-backlight/backlight/pwm-backlight/brightness");
            }
        }
        if (AS_CarplayPhone == g_Audio->getAudioSource()) {
            g_Audio->releaseAudioSource(AS_CarplayPhone);
        }
        break;
    }
    default: {
        break;
    }
    }
    m_Parent->onLinkStatusChange(CARPLAY, status);
}
