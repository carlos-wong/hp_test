#ifndef DISKDETACH_H
#define DISKDETACH_H

#include "../DiskDeviceWatcher/DiskDeviceWatcher.h"
#include <QObject>
#include <QScopedPointer>
#include <QString>

class DiskDetachPrivate;
class DiskDetach : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(DiskDetach)
public:
    explicit DiskDetach(QObject *parent = NULL);
    ~DiskDetach();
signals:
    void diskPartition(const QString partition, const DiskDeviceWatcher::Action action);
public slots:
    void onDiskDeviceChange(const QString &device, const DiskDeviceWatcher::Action action);
private:
    QScopedPointer<DiskDetachPrivate> m_Private;
};

#endif // DISKDETACH_H
