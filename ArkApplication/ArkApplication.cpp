#include "ArkApplication.h"
#include <QDebug>
#include <QStringList>
#include <QTranslator>
#include <QDesktopWidget>
#include <QTimer>
#include <QWSServer>

class ArkApplicationPrivate
{
    Q_DISABLE_COPY(ArkApplicationPrivate)
public:
    explicit ArkApplicationPrivate(int &argc, char **argv, ArkApplication* parent);
    ~ArkApplicationPrivate();
    QStringList m_Arguments;
    void initializeTranslator();
    QTranslator* m_Translator;
    QTimer* m_Timer;
private:
    ArkApplication* m_Parent;
};

ArkApplication::ArkApplication(int &argc, char **argv, QApplication::Type type)
    : QApplication(argc, argv, type)
    , m_Private(new ArkApplicationPrivate(argc, argv, this))
{
    if (QApplication::GuiServer == type) {
      //  QWSServer::setBackground(QBrush(QColor(0x00, 0x00, 0x00, 0x00)));
        m_Private->initializeTranslator();
    }
}

ArkApplication::~ArkApplication()
{
}

bool ArkApplication::installTranslatorPath(const QString &path)
{
    bool ret(false);
    if (NULL != m_Private->m_Translator) {
        ret = m_Private->m_Translator->load(path);
        if (ret) {
            installTranslator(m_Private->m_Translator);
        }
    }
    return ret;
}

int ArkApplication::size() const
{
    return m_Private->m_Arguments.size();
}

QStringList ArkApplication::arguments() const
{
    return m_Private->m_Arguments;
}

void ArkApplication::startTimer(const int second)
{
    if (NULL == m_Private->m_Timer) {
        m_Private->m_Timer = new QTimer(this);
        m_Private->m_Timer->setSingleShot(false);
        m_Private->m_Timer->start(second * 1000);
        Qt::ConnectionType type = static_cast<Qt::ConnectionType>(Qt::UniqueConnection | Qt::AutoConnection);
        QObject::connect(m_Private->m_Timer, SIGNAL(timeout()),
                         this,               SLOT(onTimeout()),
                         type);
    }
}

void ArkApplication::onTimeout()
{
    emit currentDateTime(QDateTime::currentDateTime());
}

ArkApplicationPrivate::ArkApplicationPrivate(int &argc, char **argv, ArkApplication* parent)
    : m_Parent(parent)
{
    m_Translator = NULL;
    m_Timer = NULL;
    for (int i = 0; i < argc; ++i) {
        m_Arguments << QString(argv[i]);
    }
}

ArkApplicationPrivate::~ArkApplicationPrivate()
{
}

void ArkApplicationPrivate::initializeTranslator()
{
    if (NULL == m_Translator) {
        m_Translator = new QTranslator(m_Parent);
    }
}

