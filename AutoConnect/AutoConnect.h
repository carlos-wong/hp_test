#ifndef AUTOCONNECT_H
#define AUTOCONNECT_H

#include <qnamespace.h>

class QObject;
#define ARKSENDER(arksender)  "2"#arksender""
#define ARKRECEIVER(arkreceiver) "1"#arkreceiver""
void connectSignalAndSlotByNamesake(QObject* sender, QObject* receiver, const char* read);
void connectSignalAndSignalByNamesake(QObject* sender, QObject *receiver, const char* write);

#endif // AUTOCONNECT_H
