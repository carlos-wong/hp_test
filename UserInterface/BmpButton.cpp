#include "BmpButton.h"
#include <QDebug>
#include <QPainter>
#include <QFile>
#include <QScopedPointer>
#include <QEvent>
#include <QTimer>

class BmpButtonPrivate
{
    Q_DISABLE_COPY(BmpButtonPrivate)
public:
    explicit BmpButtonPrivate(BmpButton* parent);
    ~BmpButtonPrivate();
    void initializeTimer();
    QScopedPointer<QPixmap> m_NormalPixmap;
    QScopedPointer<QPixmap> m_PressPixmap;
    QScopedPointer<QPixmap> m_CheckPixmap;
    BmpButton::ButtonStatus m_Status;
    BmpButton::Type m_LanguageType;
    QTimer* m_Timer;
    bool m_Filter;
    bool m_LongPressRestore;
private:
    BmpButton* m_Parent;
};


BmpButton::BmpButton(QWidget *parent)
    : QAbstractButton(parent)
    , m_Private(new BmpButtonPrivate(this))
{
}

BmpButton::~BmpButton()
{
}

void BmpButton::setNormalBmpPath(const QString &path)
{
    if (QFile::exists(path)) {
        m_Private->m_NormalPixmap.reset(new QPixmap(path));
        if (isVisible()) {
            update();
        }
    }
}

void BmpButton::setPressBmpPath(const QString &path)
{
    if (QFile::exists(path)) {
        m_Private->m_PressPixmap.reset(new QPixmap(path));
        if (isVisible()) {
            update();
        }
    }
}

void BmpButton::setCheckBmpPath(const QString &path)
{
    if (QFile::exists(path)) {
        m_Private->m_CheckPixmap.reset(new QPixmap(path));
        if (isVisible()) {
            update();
        }
    }
}

void BmpButton::setStatus(const BmpButton::ButtonStatus &status)
{
    if (status != m_Private->m_Status) {
        m_Private->m_Status = status;
        if (isVisible()) {
            update();
        }
    }
}

BmpButton::ButtonStatus BmpButton::getStatus()
{
    return m_Private->m_Status;
}

void BmpButton::enableLongPress(const bool flag)
{
    if (flag) {
        m_Private->initializeTimer();
    }
}

void BmpButton::enableLongPressRestore(const bool flag)
{
    m_Private->m_LongPressRestore = flag;
}

void BmpButton::setLanguageType(const BmpButton::Type type)
{
    m_Private->m_LanguageType = type;
}

bool BmpButton::event(QEvent *event)
{
    switch (event->type()) {
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseMove: {
        event->ignore();
        break;
    }
    default: {
        break;
    }
    }
    return QAbstractButton::event(event);
}

void BmpButton::mousePressEvent(QMouseEvent *event)
{
    m_Private->m_Status = BmpButton::B_Press;
    if (isVisible()) {
        update();
    }
    m_Private->m_Filter = false;
    if (NULL != m_Private->m_Timer) {
        m_Private->m_Timer->start();
    }
    //    emit bmpButtomPress();
    //    QWidget::mousePressEvent(event);
}

void BmpButton::mouseReleaseEvent(QMouseEvent *event)
{
    if (NULL != m_Private->m_Timer) {
        m_Private->m_Timer->stop();
    }
    if (isVisible()) {
        if ((NULL == m_Private->m_Timer)
                || (!m_Private->m_Filter)) {
            m_Private->m_Status = BmpButton::B_Normal;
            update();
            emit released();
        }
        //emit bmpButtonRelease();
    }
    //QWidget::mouseReleaseEvent(event);
}

void BmpButton::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    switch (m_Private->m_Status) {
    case BmpButton::B_Check: {
        checkPaint(painter);
        break;
    }
    case BmpButton::B_Press: {
        pressPaint(painter);
        break;
    }
    case BmpButton::B_Normal:
    default : {
        normalPaint(painter);
        break;
    }
    }
    if (!text().isEmpty()) {
        painter.setPen(Qt::white);
        if (BmpButton::T_Translate == m_Private->m_LanguageType) {
            painter.drawText(rect(), Qt::AlignCenter, QObject::tr(text().toLocal8Bit().constData()));
        } else {
            painter.drawText(rect(), Qt::AlignCenter, text());
        }
    }
}

void BmpButton::normalPaint(QPainter &painter)
{
    if (NULL != m_Private->m_NormalPixmap) {
        int x = (width() - m_Private->m_NormalPixmap->width()) >> 1;
        int y = (height() - m_Private->m_NormalPixmap->height()) >> 1;
        painter.drawPixmap(x, y, *m_Private->m_NormalPixmap);
    }
}

void BmpButton::pressPaint(QPainter &painter)
{
    if (NULL != m_Private->m_PressPixmap) {
        int x = (width() - m_Private->m_PressPixmap->width()) >> 1;
        int y = (height() - m_Private->m_PressPixmap->height()) >> 1;
        painter.drawPixmap(x, y, *m_Private->m_PressPixmap);
    }
}

void BmpButton::checkPaint(QPainter &painter)
{
    if (NULL != m_Private->m_CheckPixmap) {
        int x = (width() - m_Private->m_CheckPixmap->width()) >> 1;
        int y = (height() - m_Private->m_CheckPixmap->height()) >> 1;
        painter.drawPixmap(x, y, *m_Private->m_CheckPixmap);
    }
}

void BmpButton::onTimeout()
{
    if (isVisible()) {
        m_Private->m_Filter = true;
        emit longPressed();
    }
    if (m_Private->m_LongPressRestore) {
        m_Private->m_Status = BmpButton::B_Normal;
    }
    if (isVisible()) {
        update();
    }
}

BmpButtonPrivate::BmpButtonPrivate(BmpButton* parent)
    : m_Parent(parent)
{
    m_Status = BmpButton::B_Normal;
    m_LanguageType = BmpButton::T_Translate;
    m_Timer = NULL;
    m_Filter = false;
    m_LongPressRestore = true;
}

BmpButtonPrivate::~BmpButtonPrivate()
{
}

void BmpButtonPrivate::initializeTimer()
{
    if (NULL == m_Timer) {
        m_Timer = new QTimer(m_Parent);
        m_Timer->setSingleShot(true);
        m_Timer->setInterval(1500);
        QObject::connect(m_Timer,  SIGNAL(timeout()),
                         m_Parent, SLOT(onTimeout()));
    }
}
