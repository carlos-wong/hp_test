#include "MirrorLinkProxy.h"

/*
 * Implementation of interface class LocalDbusServerMirrorInterface
 */

LocalDbusServerMirrorInterface::LocalDbusServerMirrorInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent)
    : QDBusAbstractInterface(service, path, staticInterfaceName(), connection, parent)
{
}

LocalDbusServerMirrorInterface::~LocalDbusServerMirrorInterface()
{
}

