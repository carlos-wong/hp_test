#include "RunnableThread.h"
#include <QDebug>

class CustomRunnablePrivate
{
    Q_DISABLE_COPY(CustomRunnablePrivate)
public:
    explicit CustomRunnablePrivate(CustomRunnable* parent);
    ~CustomRunnablePrivate();
    void (*m_Callback)(void *);
    void *m_Parameter;
private:
    CustomRunnable* m_Parent;
};

CustomRunnable::CustomRunnable()
    : QRunnable()
    , m_Private(new CustomRunnablePrivate(this))
{
    setAutoDelete(true);
}

CustomRunnable::~CustomRunnable()
{
}

void CustomRunnable::setCallbackFunction(void (*callback)(void *), void *parameter)
{
    m_Private->m_Callback = callback;
    m_Private->m_Parameter = parameter;
}

void CustomRunnable::run()
{
    if (NULL != m_Private->m_Callback) {
        m_Private->m_Callback(m_Private->m_Parameter);
    }
}

CustomRunnablePrivate::CustomRunnablePrivate(CustomRunnable *parent)
    : m_Parent(parent)
{
    m_Callback = NULL;
    m_Parameter = NULL;
}

CustomRunnablePrivate::~CustomRunnablePrivate()
{
}

class CustomThreadPrivate
{
    Q_DISABLE_COPY(CustomThreadPrivate)
public:
    explicit CustomThreadPrivate(CustomThread* parent);
    ~CustomThreadPrivate();
    void (*m_Callback)(void *);
    void *m_Parameter;
    bool m_EventLoop;
private:
    CustomThread* m_Parent;
};

CustomThread::CustomThread(QObject *parent)
    : QThread(parent)
    , m_Private(new CustomThreadPrivate(this))
{
}

CustomThread::~CustomThread()
{
}

void CustomThread::setCallbackFunction(void (*callback)(void *), void *parameter)
{
    m_Private->m_Callback = callback;
    m_Private->m_Parameter = parameter;
}

void CustomThread::setEventLoop(const bool exec)
{
    m_Private->m_EventLoop = exec;
}

void CustomThread::setusleep(const unsigned long value)
{
    usleep(value);
}

void CustomThread::callbackFunctionSlots()
{
    if (NULL != m_Private->m_Callback) {
        m_Private->m_Callback(m_Private->m_Parameter);
    }
}

void CustomThread::run()
{
    if (m_Private->m_EventLoop) {
        exec();
    } else {
        if (NULL != m_Private->m_Callback) {
            m_Private->m_Callback(m_Private->m_Parameter);
        }
    }
}

CustomThreadPrivate::CustomThreadPrivate(CustomThread* parent)
    : m_Parent(parent)
{
    m_Callback = NULL;
    m_Parameter = NULL;
    m_EventLoop = false;
}

CustomThreadPrivate::~CustomThreadPrivate()
{
}

