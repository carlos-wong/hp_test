#include "CarplayLinkProxy.h"

/*
 * Implementation of interface class LocalDbusServerCarplayInterface
 */

LocalDbusServerCarplayInterface::LocalDbusServerCarplayInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent)
    : QDBusAbstractInterface(service, path, staticInterfaceName(), connection, parent)
{
}

LocalDbusServerCarplayInterface::~LocalDbusServerCarplayInterface()
{
}

