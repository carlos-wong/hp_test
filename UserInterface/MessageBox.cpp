#include "MessageBox.h"
#include "BmpWidget.h"
#include "UserInterfaceUtility.h"
#include <QFile>
#include <QPainter>
#include <QTimer>
#include <QFont>

class MessageBoxPrivate
{
    Q_DISABLE_COPY(MessageBoxPrivate)
public:
    explicit MessageBoxPrivate(MessageBox *parent);
    ~MessageBoxPrivate();
    void initialize();
    void connectAllSlots();
    QString m_Text;
    QTimer* m_Timer;
    QScopedPointer<QPixmap> m_BackgroundPixmap;
    QFont m_Font;
    bool m_AutoHide;
private:
    MessageBox *m_Parent;
};

MessageBox::MessageBox(QWidget *parent)
    : QWidget(parent)
    , m_Private(new MessageBoxPrivate(this))
{
}

MessageBox::~MessageBox()
{
}

void MessageBox::setText(const QString &text)
{
    if (text != m_Private->m_Text) {
        m_Private->m_Text = text;
        if (isVisible()) {
            update();
        }
    }
}

void MessageBox::setAutoHide(const bool flag)
{
    if (flag != m_Private->m_AutoHide) {
        m_Private->m_AutoHide = flag;
    }
}

void MessageBox::setBackgroundBmpPath(const QString &path)
{
    if (QFile::exists(path)) {
        m_Private->m_BackgroundPixmap.reset(new QPixmap(path));
    }
}

void MessageBox::setFontPointSize(const int pointSize)
{
    m_Private->m_Font.setPointSize(pointSize);
    if (isVisible()) {
        update();
    }
}

void MessageBox::showEvent(QShowEvent *event)
{
    m_Private->m_Timer->start();
    emit messageWidgetChange(MessageBox::T_Show);
    QWidget::showEvent(event);
}

void MessageBox::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
}

void MessageBox::hideEvent(QHideEvent *event)
{
    if (isVisible()) {
        if (m_Private->m_Timer->isActive()) {
            m_Private->m_Timer->stop();
        }
        emit messageWidgetChange(MessageBox::T_Hide);
    }
    QWidget::hideEvent(event);
}

void MessageBox::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.fillRect(rect(), QColor(0, 0, 0, 80));
    if (!m_Private->m_BackgroundPixmap.isNull()) {
        int x = (width() - m_Private->m_BackgroundPixmap->width()) >> 1;
        int y = (height() - m_Private->m_BackgroundPixmap->height()) >> 1;
        painter.drawPixmap(x, y, *m_Private->m_BackgroundPixmap);
    }
    if (!m_Private->m_Text.isEmpty()) {
        painter.setFont(m_Private->m_Font);
        painter.setPen(Qt::white);
        painter.drawText(rect(), Qt::AlignCenter, QObject::tr(m_Private->m_Text.toLocal8Bit().constData()));
    }
    QWidget::paintEvent(event);
}

void MessageBox::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_Private->m_AutoHide && isVisible()) {
        setVisible(false);
    }
    QWidget::mouseReleaseEvent(event);
}

MessageBoxPrivate::MessageBoxPrivate(MessageBox *parent)
    : m_Parent(parent)
{
    m_Timer = NULL;
    m_AutoHide = true;
    initialize();
    connectAllSlots();
}

MessageBoxPrivate::~MessageBoxPrivate()
{
}

void MessageBoxPrivate::initialize()
{
    m_BackgroundPixmap.reset(NULL);
    m_Text.clear();
    m_Timer = new QTimer(m_Parent);
    m_Timer->setSingleShot(true);
    m_Timer->setInterval(3000);
}

void MessageBoxPrivate::connectAllSlots()
{
    Qt::ConnectionType type = static_cast<Qt::ConnectionType>(Qt::UniqueConnection | Qt::AutoConnection);
    QObject::connect(m_Timer,  SIGNAL(timeout()),
                     m_Parent, SLOT(onTimeOut()),
                     type);
}

void MessageBox::onTimeOut()
{
    if (m_Private->m_AutoHide && isVisible()) {
        setVisible(false);
    }
    emit messageShowTimeout();
}
