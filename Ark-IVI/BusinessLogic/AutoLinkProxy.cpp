#include "AutoLinkProxy.h"

/*
 * Implementation of interface class LocalDbusServerAutoInterface
 */

LocalDbusServerAutoInterface::LocalDbusServerAutoInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent)
    : QDBusAbstractInterface(service, path, staticInterfaceName(), connection, parent)
{
}

LocalDbusServerAutoInterface::~LocalDbusServerAutoInterface()
{
}


