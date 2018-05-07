#include "AutoConnect.h"
#include <QObject>

void connectSignalAndSlotByNamesake(QObject *sender, QObject *receiver, const char *read)
{
    QObject::connect(sender, QString(read).replace(QChar('1'), QChar('2')).toLocal8Bit().constData(), receiver, read, Qt::UniqueConnection);
}

void connectSignalAndSignalByNamesake(QObject *sender, QObject *receiver, const char *write)
{
    QObject::connect(sender, write, receiver, write, Qt::UniqueConnection);
}
