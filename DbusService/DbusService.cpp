#include "DbusService.h"
#include "RunnableThread.h"
#include <QDebug>
#include <QDBusServiceWatcher>
#include <QThreadPool>
#include <QDBusReply>
#include <QDBusConnection>
#include <QDBusConnectionInterface>

class DbusServicePrivate
{
public:
    explicit DbusServicePrivate(DbusService* parent);
    ~DbusServicePrivate();
    void initialize();
    void connectAllSlots();
    QDBusServiceWatcher* m_DBusServiceWatcher;
private:
    DbusService* m_Parent;
};

static void initializeRunnableCallback(void *paramater)
{
    qDebug() << __FILE__ << __FUNCTION__;
    QString* ptr = static_cast<QString*>(paramater);
    if (NULL != ptr) {
        QDBusReply<void> reply = QDBusConnection::sessionBus().interface()->startService(*ptr);
        if (!reply.isValid()) {
            qCritical() << "DbusService::startService:Error" << reply.error().message();
        } else {
            qDebug() << "DbusService::startService" << *ptr << "success!";
        }
        delete ptr;
        ptr = NULL;
    }
}

bool DbusService::startService(const QString &service)
{
    QString* ptr = new QString(service);
    CustomRunnable* runnable = new CustomRunnable();
    runnable->setCallbackFunction(initializeRunnableCallback, static_cast<void*>(ptr));
    QThreadPool::globalInstance()->start(runnable);
    m_Private->m_DBusServiceWatcher->addWatchedService(service);
    return true;
}

void DbusService::addWatchedService(const QString &service)
{
    m_Private->m_DBusServiceWatcher->addWatchedService(service);
}

bool DbusService::removeWatchedService(const QString &service)
{
    return m_Private->m_DBusServiceWatcher->removeWatchedService(service);
}

DbusService::DbusService(QObject *parent)
    : QObject(parent)
    , m_Private(new DbusServicePrivate(this))
{
}

DbusService::~DbusService()
{
}

DbusServicePrivate::DbusServicePrivate(DbusService *parent)
    : m_Parent(parent)
{
    m_DBusServiceWatcher = NULL;
    initialize();
    connectAllSlots();
}

DbusServicePrivate::~DbusServicePrivate()
{
}

void DbusServicePrivate::initialize()
{
    m_DBusServiceWatcher = new QDBusServiceWatcher(m_Parent);
    m_DBusServiceWatcher->setConnection(QDBusConnection::sessionBus());
}

void DbusServicePrivate::connectAllSlots()
{
    Qt::ConnectionType type = static_cast<Qt::ConnectionType>(Qt::AutoConnection | Qt::UniqueConnection);
    QObject::connect(m_DBusServiceWatcher, SIGNAL(serviceRegistered(const QString &)),
                     m_Parent,             SIGNAL(onServiceRegistered(const QString &)),
                     type);
    QObject::connect(m_DBusServiceWatcher, SIGNAL(serviceUnregistered(const QString &)),
                     m_Parent,             SIGNAL(onServiceUnregistered(const QString &)),
                     type);
}
