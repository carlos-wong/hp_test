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
    bool m_AutoFillBackground;
    bool m_BorderVisible;
    QColor m_BorderCheckedColor;
    QColor m_BorderNormalColor;
    int m_BorderWidth;
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

void BmpButton::setImageAutoFillBackground(bool flag)
{
    m_Private->m_AutoFillBackground = flag;
}

void BmpButton::setBorderVisible(bool flag)
{
    m_Private->m_BorderVisible = flag;
}

void BmpButton::setBorderStyle(const QColor &checkColor, const QColor &normalColor, int w)
{
    m_Private->m_BorderCheckedColor = checkColor;
    m_Private->m_BorderNormalColor = normalColor;
    m_Private->m_BorderWidth = w;
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
        if (!m_Private->m_BorderVisible) {
            if (!m_Private->m_AutoFillBackground) {
                int x = (width() - m_Private->m_NormalPixmap->width()) >> 1;
                int y = (height() - m_Private->m_NormalPixmap->height()) >> 1;
                painter.drawPixmap(x, y, *m_Private->m_NormalPixmap);
            } else {
                painter.drawPixmap(this->rect(), *m_Private->m_NormalPixmap);
            }
        }
        else {
            int w = m_Private->m_BorderWidth;
            painter.setPen(QPen(QBrush(m_Private->m_BorderNormalColor), w));
            painter.drawRect(this->rect());
            painter.drawPixmap(w, w, this->width()-w*2, this->height()-w*2, *m_Private->m_NormalPixmap);
        }
    }
}

void BmpButton::pressPaint(QPainter &painter)
{
    if (NULL != m_Private->m_PressPixmap) {
        if (!m_Private->m_BorderVisible) {
            if (!m_Private->m_AutoFillBackground) {
                int x = (width() - m_Private->m_NormalPixmap->width()) >> 1;
                int y = (height() - m_Private->m_NormalPixmap->height()) >> 1;
                painter.drawPixmap(x, y, *m_Private->m_PressPixmap);
            } else {
                painter.drawPixmap(this->rect(), *m_Private->m_PressPixmap);
            }
        }
        else {
            int w = m_Private->m_BorderWidth;
            painter.setPen(QPen(QBrush(m_Private->m_BorderCheckedColor), w));
            painter.drawRect(this->rect());
            painter.drawPixmap(w, w, this->width()-w*2, this->height()-w*2, *m_Private->m_PressPixmap);
        }
    }
}

void BmpButton::checkPaint(QPainter &painter)
{
    if (NULL != m_Private->m_CheckPixmap) {
        if (!m_Private->m_BorderVisible) {
            if (!m_Private->m_AutoFillBackground) {
                int x = (width() - m_Private->m_NormalPixmap->width()) >> 1;
                int y = (height() - m_Private->m_NormalPixmap->height()) >> 1;
                painter.drawPixmap(x, y, *m_Private->m_CheckPixmap);
            } else {
                painter.drawPixmap(this->rect(), *m_Private->m_CheckPixmap);
            }
        }
        else {
            int w = m_Private->m_BorderWidth;
            painter.setPen(QPen(QBrush(m_Private->m_BorderCheckedColor), w));
            painter.drawRect(this->rect());
            painter.drawPixmap(w, w, this->width()-w*2, this->height()-w*2, *m_Private->m_CheckPixmap);
        }
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
    m_AutoFillBackground = false;
    m_BorderVisible = false;
    m_BorderCheckedColor.setRgb(0,168,255);
    m_BorderNormalColor.setRgb(255, 255, 255);
    m_BorderWidth = 2;
}

BmpButtonPrivate::~BmpButtonPrivate()
{
}

void BmpButtonPrivate::initializeTimer()
{
    if (NULL == m_Timer) {
        m_Timer = new QTimer(m_Parent);
        m_Timer->setSingleShot(true);
       // m_Timer->setInterval(1500);
        m_Timer->setInterval(800);
        QObject::connect(m_Timer,  SIGNAL(timeout()),
                         m_Parent, SLOT(onTimeout()));
    }
}
