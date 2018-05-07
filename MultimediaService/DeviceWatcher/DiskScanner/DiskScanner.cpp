#include "DiskScanner.h"
#include "RunnableThread.h"
#include "UserInterfaceUtility.h"
#include <QTimer>
#include <QDir>
#include <QMutex>
#include <QDirIterator>
#include <QCoreApplication>
#include <QMutexLocker>
#include <QQueue>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

static void scannerThreadCallback(void *paramater);

class DiskScannerPrivate
{
    Q_DISABLE_COPY(DiskScannerPrivate)
public:
    explicit DiskScannerPrivate(DiskScanner *parent);
    ~DiskScannerPrivate();
    void initialize();
    void connectAllSlots();
    bool recursionScan(const QString& path);
    void scanFinish();
    CustomThread* m_CustomThread;
    QMap<int, QStringList> m_FilterMapList;
    QStringList m_Filter;
    QTimer* m_Timer;
    QString m_Path;
    volatile bool m_ContinueFlag;
    QMutex m_Mutex;
private:
    DiskScanner* m_Parent;
};

DiskScanner::DiskScanner(QObject *parent)
    : QObject(parent)
    , m_Private(new DiskScannerPrivate(this))
{   
}

DiskScanner::~DiskScanner()
{
}

void DiskScanner::setScannerSuffixMap(const QMap<int, QStringList> &mapList)
{
    m_Private->m_FilterMapList = mapList;
    for (QMap< int, QStringList >::iterator suffixIter = m_Private->m_FilterMapList.begin();
         suffixIter != m_Private->m_FilterMapList.end();
         ++suffixIter) {
        for (int i = 0; i < suffixIter.value().size(); ++i) {
            m_Private->m_Filter.append(QString("*") + suffixIter.value().at(i));
        }
    }
}

void DiskScanner::setMinimumScanTime(const int msec)
{
    m_Private->m_Timer->setInterval(msec);
}

void DiskScanner::startScanner(const QString &path)
{
    QMutexLocker mutexLocker(&m_Private->m_Mutex);
    m_Private->m_Timer->start();
    m_Private->m_Path = path;
    m_Private->m_ContinueFlag = true;
    m_Private->m_CustomThread->start(QThread::IdlePriority);
}

void DiskScanner::stopScanner()
{
    QMutexLocker mutexLocker(&m_Private->m_Mutex);
    m_Private->m_Path.clear();
    m_Private->m_Timer->stop();
    m_Private->m_ContinueFlag = false;
    if (m_Private->m_CustomThread->isRunning()) {
        m_Private->m_CustomThread->wait();
    }
}

void DiskScanner::onFinish()
{
    if (!m_Private->m_Timer->isActive()) {
        onTimeout();
    }
}

void DiskScanner::onTimeout()
{
    if (!m_Private->m_CustomThread->isRunning()) {
        m_Private->scanFinish();
    }
}

DiskScannerPrivate::DiskScannerPrivate(DiskScanner *parent)
    : m_Parent(parent)
{
    m_CustomThread = NULL;
    m_Timer = NULL;
    m_ContinueFlag = true;
    initialize();
    connectAllSlots();
}

DiskScannerPrivate::~DiskScannerPrivate()
{
    m_CustomThread->exit(0);
    m_CustomThread->wait();
}

void DiskScannerPrivate::initialize()
{
    m_CustomThread = new CustomThread(m_Parent);
    m_CustomThread->setCallbackFunction(scannerThreadCallback, static_cast<void *>(this));
    m_Timer = new QTimer(m_Parent);
    m_Timer->setSingleShot(true);
    m_Timer->setInterval(3500);
}

void DiskScannerPrivate::connectAllSlots()
{
    Qt::ConnectionType type = static_cast<Qt::ConnectionType>(Qt::UniqueConnection | Qt::AutoConnection);
    QObject::connect(m_CustomThread, SIGNAL(finished()),
                     m_Parent,       SLOT(onFinish()),
                     type);
    QObject::connect(m_Timer,  SIGNAL(timeout()),
                     m_Parent, SLOT(onTimeout()),
                     type);
}

bool DiskScannerPrivate::recursionScan(const QString& path)
{
    QString dirPath(path);
    QQueue<QString> queue;
    queue.enqueue(dirPath);
    DIR* dirptr = NULL;
    struct dirent* direntptr = NULL;
    struct stat64 statbuff;
    QString fullname;
    QString splliter("/");
    while (!queue.isEmpty()) {
        dirPath = queue.dequeue() + splliter;
        if (NULL != (dirptr = opendir(dirPath.toLocal8Bit().data()))) {
            while (NULL != (direntptr = readdir(dirptr))) {
                if ((NULL != direntptr->d_name)
                        && ('.' == direntptr->d_name[0])) {
                    continue;
                } else {
                    fullname = dirPath + QString(direntptr->d_name);
                    if (-1 != stat64(fullname.toLocal8Bit().data(), &statbuff)) {
                        if (S_ISDIR(statbuff.st_mode)) {
                            queue.enqueue(QString(fullname));
                        } else {
                            for (QMap< int, QStringList >::iterator suffixIter = m_FilterMapList.begin();
                                 suffixIter != m_FilterMapList.end();
                                 ++suffixIter) {
                                if (m_ContinueFlag) {
                                    if (suffixIter.value().contains(QString(strrchr(fullname.toLocal8Bit().data(), '.')), Qt::CaseInsensitive)) {
                                        emit m_Parent->filePath(fullname, suffixIter.key());
                                    }
                                } else {
                                    break;
                                }
                            }
                        }
                    } else {
                        qWarning() << "state64 error";
                        break;
                    }
                }
            }
            if (0 == closedir(dirptr)) {
                dirptr = NULL;
            } else {
                qWarning() << "closedir error";
                break;
            }
        } else {
            qWarning() << "opendir failed";
            break;
        }
    }
    return 0;
}

void DiskScannerPrivate::scanFinish()
{
    if (!m_Timer->isActive()) {
        QMutexLocker mutexLocker(&m_Mutex);
        if (!m_Path.isEmpty()) {
            emit m_Parent->finish();
        }
        m_Path.clear();
        m_Timer->stop();
        m_ContinueFlag = false;
    }
}

static void scannerThreadCallback(void *paramater)
{
    DiskScannerPrivate *ptr = static_cast<DiskScannerPrivate *>(paramater);
    if (NULL != ptr) {
        if (ptr->m_ContinueFlag) {
            ptr->recursionScan(ptr->m_Path);
            if (ptr->m_ContinueFlag) {
                ptr->scanFinish();
            }
        }
    }
}
