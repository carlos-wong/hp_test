#include "DiskDetach.h"
#include "UserInterfaceUtility.h"
#include <QTimer>
#include <QRegExp>
#include <QFile>

class DiskDetachPrivate
{
    Q_DISABLE_COPY(DiskDetachPrivate)
public:
    explicit DiskDetachPrivate(DiskDetach* parent);
    ~DiskDetachPrivate();
    void initialize();
    void connectAllSlots();
    bool regExpUSBPath(const QString& path);
    bool regExpSDPath(const QString& path);
    QString m_USBPartition;
    QString m_SDPartition;
    DiskDeviceWatcher* m_DiskDeviceWatcher;
private:
    DiskDetach* m_Parent;
};

DiskDetach::DiskDetach(QObject *parent)
    : QObject(parent)
    , m_Private(new DiskDetachPrivate(this))
{
}

DiskDetach::~DiskDetach()
{
}

void DiskDetach::onDiskDeviceChange(const QString &device, const DiskDeviceWatcher::Action action)
{
    qWarning() << __PRETTY_FUNCTION__ << device << action;
    if (DiskDeviceWatcher::A_Add == action) {
        bool condition1(false);
        bool condition2(false);
        bool condition3(false);
        if (m_Private->regExpUSBPath(device)) {
            condition1 = m_Private->m_USBPartition.isEmpty();
            if (!condition1
                    && (device.length() > 0)) {
                condition2 = device.at(device.length() - 1).isNumber();
                condition3 = condition2 && (device.at(device.length() - 1) > QChar('0'));
            }
            if (condition1
                    || condition2
                    || condition3) {
                if ((condition1) || (m_Private->m_USBPartition.length() > 0)
                        && ((!m_Private->m_USBPartition.at(m_Private->m_USBPartition.length() - 1).isNumber())
                            || (m_Private->m_USBPartition.at(m_Private->m_USBPartition.length() - 1).isNumber()
                                && (!(m_Private->m_USBPartition.at(m_Private->m_USBPartition.length() - 1) > QChar('0')))))) {
                    m_Private->m_USBPartition = device;
                    for (int i = 1; i < 10; ++i) {
                        if (QFile::exists(device + QString::number(i))) {
                            m_Private->m_USBPartition = device + QString::number(i);
                            break;
                        }
                    }
                    emit diskPartition(m_Private->m_USBPartition, DiskDeviceWatcher::A_Add);
                }
            }
        } else if (m_Private->regExpSDPath(device)) {
            condition1 = m_Private->m_SDPartition.isEmpty();
            if (!condition1
                    && (device.length() > 0)) {
                condition2 = device.at(device.length() - 1).isNumber();
                condition3 = condition2 && (device.at(device.length() - 1) > QChar('0'));
            }
            if (condition1
                    || condition2
                    || condition3) {
                if ((condition1) || (m_Private->m_SDPartition.length() > 0)
                        && ((!m_Private->m_SDPartition.at(m_Private->m_SDPartition.length() - 1).isNumber())
                            || (m_Private->m_SDPartition.at(m_Private->m_SDPartition.length() - 1).isNumber()
                                && (!(m_Private->m_SDPartition.at(m_Private->m_SDPartition.length() - 1) > QChar('0')))))) {
                    m_Private->m_SDPartition = device;
                    for (int i = 1; i < 10; ++i) {
                        if (QFile::exists(device + QString("p") + QString::number(i))) {
                            m_Private->m_SDPartition = device + QString("p") + QString::number(i);
                            break;
                        }
                    }
                    qWarning() << __PRETTY_FUNCTION__ << __LINE__ << m_Private->m_SDPartition << "add";
                    emit diskPartition(m_Private->m_SDPartition, DiskDeviceWatcher::A_Add);
                }
            }
        }
    } else if (DiskDeviceWatcher::A_Remove == action) {
        if ((m_Private->regExpUSBPath(device))
                && (device == m_Private->m_USBPartition)) {
            emit diskPartition(m_Private->m_USBPartition, DiskDeviceWatcher::A_Remove);
            m_Private->m_USBPartition.clear();
        } else if ((m_Private->regExpSDPath(device))
                   && (device == m_Private->m_SDPartition)) {
            emit diskPartition(m_Private->m_SDPartition, DiskDeviceWatcher::A_Remove);
            m_Private->m_SDPartition.clear();
        }
    }
}

DiskDetachPrivate::DiskDetachPrivate(DiskDetach *parent)
    : m_Parent(parent)
{
    m_DiskDeviceWatcher = NULL;
    initialize();
    connectAllSlots();
}

DiskDetachPrivate::~DiskDetachPrivate()
{
}

void DiskDetachPrivate::initialize()
{
    m_DiskDeviceWatcher = new DiskDeviceWatcher(m_Parent);
}

void DiskDetachPrivate::connectAllSlots()
{
    Qt::ConnectionType type = static_cast<Qt::ConnectionType>(Qt::AutoConnection | Qt::UniqueConnection);
    QObject::connect(m_DiskDeviceWatcher, SIGNAL(diskDeviceChange(const QString &, const DiskDeviceWatcher::Action)),
                     m_Parent,            SLOT(onDiskDeviceChange(const QString &, const DiskDeviceWatcher::Action)),
                     type);
}

bool DiskDetachPrivate::regExpUSBPath(const QString &path)
{
    return path.startsWith(QString("/dev/sd"));
}

bool DiskDetachPrivate::regExpSDPath(const QString &path)
{
    return (path.startsWith(QString("/dev/mmcblk")) && 1/*path.contains(QChar('p'))*/);
//    return (path.startsWith(QString("/dev/mmcblk")) && path.contains(QChar('p')));
}
