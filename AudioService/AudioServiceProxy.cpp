#include "AudioServiceProxy.h"

/*
 * Implementation of interface class LocalDbusServerAudioInterface
 */

LocalDbusServerAudioInterface::LocalDbusServerAudioInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent)
    : QDBusAbstractInterface(service, path, staticInterfaceName(), connection, parent)
{
}

LocalDbusServerAudioInterface::~LocalDbusServerAudioInterface()
{
}

