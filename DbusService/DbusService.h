#ifndef DBUSSERVICE_H
#define DBUSSERVICE_H

#include <QObject>
#include <QScopedPointer>
#include <QCoreApplication>

class DbusServicePrivate;
class DbusService : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(DbusService)
#ifdef g_DbusService
#undef g_DbusService
#endif
#define g_DbusService (DbusService::instance())
public:
    inline static DbusService* instance() {
        static DbusService* dbusService(new DbusService(qApp));
        return dbusService;
    }
    bool startService(const QString& service);
    void addWatchedService(const QString &service);
    bool removeWatchedService(const QString &service);
signals:
    void onServiceRegistered(const QString &service);
    void onServiceUnregistered(const QString &service);
private:
    explicit DbusService(QObject* parent = NULL);
    ~DbusService();
    friend class DbusServicePrivate;
    QScopedPointer<DbusServicePrivate> m_Private;
};

#endif // DBUSSERVICE_H
