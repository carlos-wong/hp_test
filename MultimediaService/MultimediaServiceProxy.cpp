#include "MultimediaServiceProxy.h"

/*
 * Implementation of interface class LocalDbusServerMultimediaInterface
 */

LocalDbusServerMultimediaInterface::LocalDbusServerMultimediaInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent)
    : QDBusAbstractInterface(service, path, staticInterfaceName(), connection, parent)
{
}

LocalDbusServerMultimediaInterface::~LocalDbusServerMultimediaInterface()
{
}

