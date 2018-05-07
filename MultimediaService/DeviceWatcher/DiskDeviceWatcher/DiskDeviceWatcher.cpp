#include "DiskDeviceWatcher.h"
#include <QDebug>
#include <QSocketNotifier>
#include <QByteArray>
#include <QRegExp>
#include <QBuffer>
#include <QCoreApplication>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/types.h>
#include <linux/netlink.h>
#include <errno.h>
#include <unistd.h>

#define UEVENT_BUFFER_SIZE      2048
#define UDEV_MONITOR_KERNEL     1
//#define USE_REGEXP 1

class DiskDeviceWatcherPrivate
{
    Q_DISABLE_COPY(DiskDeviceWatcherPrivate)
public:
    explicit DiskDeviceWatcherPrivate(DiskDeviceWatcher* parent);
    ~DiskDeviceWatcherPrivate();
    void initialize();
    void parseLine(const QByteArray& line);
    QSocketNotifier* m_SocketNotifier;
    QBuffer m_Buffer;
    int m_NetLinkSocket;
private:
    DiskDeviceWatcher* m_Parent;
};

DiskDeviceWatcher::DiskDeviceWatcher(QObject *parent)
    : QObject(parent)
    , m_Private(new DiskDeviceWatcherPrivate(this))
{
}

DiskDeviceWatcher::~DiskDeviceWatcher()
{
}

void DiskDeviceWatcher::onSocketNotifier()
{
    QByteArray data;
    data.resize(UEVENT_BUFFER_SIZE * 2);
    data.fill(0);
    size_t len = read(m_Private->m_SocketNotifier->socket(), data.data(), UEVENT_BUFFER_SIZE * 2);
    data.resize(len);
    data = data.replace(0, '\n').trimmed();
    if (m_Private->m_Buffer.isOpen()) {
        m_Private->m_Buffer.close();
    }
    m_Private->m_Buffer.setBuffer(&data);
    if (m_Private->m_Buffer.open(QIODevice::ReadOnly)) {
        while(m_Private->m_Buffer.canReadLine()) {
            m_Private->parseLine(m_Private->m_Buffer.readLine().trimmed());
        }
        m_Private->m_Buffer.close();
    }
}

DiskDeviceWatcherPrivate::DiskDeviceWatcherPrivate(DiskDeviceWatcher *parent)
    : m_Parent(parent)
{
    m_SocketNotifier = NULL;
    m_NetLinkSocket = -1;
    initialize();
}

DiskDeviceWatcherPrivate::~DiskDeviceWatcherPrivate()
{
}

void DiskDeviceWatcherPrivate::initialize()
{
    struct sockaddr_nl snl;
    const int buffersize = 16 * 1024 * 1024;
    int retval;

    memset(&snl, 0x00, sizeof(struct sockaddr_nl));
    snl.nl_family = AF_NETLINK;
    snl.nl_pid = getpid();
    snl.nl_groups = UDEV_MONITOR_KERNEL;

    m_NetLinkSocket = socket(PF_NETLINK, SOCK_RAW, NETLINK_KOBJECT_UEVENT);
    if (-1 == m_NetLinkSocket) {
        exit(EXIT_FAILURE);
    }

    /* set receive buffersize */
    setsockopt(m_NetLinkSocket, SOL_SOCKET, SO_RCVBUFFORCE, &buffersize, sizeof(buffersize));
    retval = bind(m_NetLinkSocket, (struct sockaddr*) &snl, sizeof(struct sockaddr_nl));
    if (retval < 0) {
        close(m_NetLinkSocket);
        m_NetLinkSocket = -1;
        exit(EXIT_FAILURE);
    } else if (retval == 0) {
        struct sockaddr_nl _snl;
        socklen_t _addrlen;
        _addrlen = sizeof(struct sockaddr_nl);
        retval = getsockname(m_NetLinkSocket, (struct sockaddr *)&_snl, &_addrlen);
        if (0 == retval) {
            snl.nl_pid = _snl.nl_pid;
        }
    }
    m_SocketNotifier = new QSocketNotifier(m_NetLinkSocket, QSocketNotifier::Read, m_Parent);
    Qt::ConnectionType type = static_cast<Qt::ConnectionType>(Qt::UniqueConnection | Qt::AutoConnection);
    QObject::connect(m_SocketNotifier, SIGNAL(activated(int)),
                     m_Parent,         SLOT(onSocketNotifier()),
                     type); //will always active
    m_SocketNotifier->setEnabled(true);
}

void DiskDeviceWatcherPrivate::parseLine(const QByteArray &line)
{
    if (line.contains("/block/")) { //hotplug
        QString action_str = line.left(line.indexOf('@')).toLower();
        QString dev = "/dev/" + line.right(line.length() - line.lastIndexOf('/') - 1);
        DiskDeviceWatcher::Action action;
        if (QString(qgetenv("PROTOCOL_ID")).contains(QString("yaoxi"))) {
            if (line.contains("musb-ark1680")) {
                if (!line.contains("musb-ark1680.1")) {
                    return ;
                }
            }
        }
        if (QString("add") == action_str) {
            action = DiskDeviceWatcher::A_Add;
            emit m_Parent->diskDeviceChange(dev, action);
        } else if (QString("remove") == action_str) {
            action = DiskDeviceWatcher::A_Remove;
            emit m_Parent->diskDeviceChange(dev, action);
        }
    }
}
