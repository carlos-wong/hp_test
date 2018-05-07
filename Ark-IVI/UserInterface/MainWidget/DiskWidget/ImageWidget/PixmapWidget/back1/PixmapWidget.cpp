#include "PixmapWidget.h"
#include "BusinessLogic/Widget.h"
#include "AutoConnect.h"
#include "UserInterfaceUtility.h"
#include <QPainter>
#include <QMouseEvent>
#include <QImage>
#include <QtGlobal>

class PixmapWidgetPrivate
{
    Q_DISABLE_COPY(PixmapWidgetPrivate)
public:
    explicit PixmapWidgetPrivate(PixmapWidget* parent);
    ~PixmapWidgetPrivate();
    void connectAllSlots();
    QPixmap m_Pixmap;
    QImage m_Image;
    int m_X1 = 0;
    int m_Y1 = 0;
    int m_DeltaX = 0;
    int m_DeltaY = 0;
    int m_OldWidth = 0;
    int m_OldHeight = 0;
    bool m_MouseFilter = false;
    bool m_ChangeFilter = false;
private:
    PixmapWidget* m_Parent = nullptr;
};

PixmapWidget::PixmapWidget(QWidget *parent)
    : QWidget(parent)
    , m_Private(new PixmapWidgetPrivate(this))
{
}

PixmapWidget::~PixmapWidget()
{
}

void PixmapWidget::onPixmapChange(const QPixmap &pixmap, const bool change)
{
    qDebug() << "PixmapWidget::onPixmapChange" << change << pixmap.width() << pixmap.height();
    if (change) {

    } else {
        m_Private->m_MouseFilter = false;
        m_Private->m_Pixmap = pixmap;
        int x(this->x());
        m_Private->m_X1 = 0;
        if (m_Private->m_Pixmap.width() <= parentWidget()->width()) {
            m_Private->m_X1 = 0;
            m_Private->m_DeltaX = 0;
            x = (parentWidget()->width() - m_Private->m_Pixmap.width()) * 0.5;
        } else {
            if (m_Private->m_Pixmap.width() != m_Private->m_OldWidth) {
                x = (parentWidget()->width() - m_Private->m_Pixmap.width()) * 0.5;
                float factorX = (m_Private->m_DeltaX / static_cast<float>(m_Private->m_OldWidth));
                if (m_Private->m_Pixmap.width() < m_Private->m_OldWidth) {
                    m_Private->m_DeltaX = m_Private->m_DeltaX * (1 + factorX);
                    if (x <= 0) {
                        if (m_Private->m_DeltaX > 0) {
                            if (m_Private->m_DeltaX > qAbs(x)) {
                                m_Private->m_DeltaX = -x;
                            }
                        } else {
                            if (m_Private->m_DeltaX < x) {
                                m_Private->m_DeltaX = x;
                            }
                        }
                    }
                } else if (m_Private->m_Pixmap.width() > m_Private->m_OldWidth) {
                    m_Private->m_DeltaX = factorX * m_Private->m_Pixmap.width();
                }
            }
        }
        m_Private->m_OldWidth = m_Private->m_Pixmap.width();

        int y(this->y());
        if (m_Private->m_Pixmap.height() <= parentWidget()->height()) {
            m_Private->m_Y1 = 0;
            m_Private->m_DeltaY = 0;
            y = (parentWidget()->height() - m_Private->m_Pixmap.height()) * 0.5;
        } else {
            if (m_Private->m_Pixmap.height() != m_Private->m_OldHeight) {
                y = (parentWidget()->height() - m_Private->m_Pixmap.height()) * 0.5;
                float factorY = (m_Private->m_DeltaY / static_cast<float>(m_Private->m_OldHeight));
                if (m_Private->m_Pixmap.height() < m_Private->m_OldHeight) {
                    m_Private->m_DeltaY = m_Private->m_DeltaY * (1 + factorY);
                    if (y <= 0) {
                        if (m_Private->m_DeltaY >= 0) {
                            if (m_Private->m_DeltaY > qAbs(y)) {
                                m_Private->m_DeltaY = -y;
                            }
                        } else {
                            if (m_Private->m_DeltaY < y) {
                                m_Private->m_DeltaY = y;
                            }
                        }
                    }
                } else if (m_Private->m_Pixmap.height() > m_Private->m_OldHeight) {
                    m_Private->m_DeltaY = factorY * m_Private->m_Pixmap.height();
                }
            }
        }
        m_Private->m_OldHeight = m_Private->m_Pixmap.height();
        setGeometry(x, y, m_Private->m_Pixmap.width(), m_Private->m_Pixmap.height());
        update();
    }
}

void PixmapWidget::onImageChange(const QImage &image, const bool change)
{
    if (change) {
        m_Private->m_X1 = 0;
        m_Private->m_Y1 = 0;
        m_Private->m_DeltaX = 0;
        m_Private->m_DeltaY = 0;
        m_Private->m_OldWidth = 0;
        m_Private->m_OldHeight = 0;
        m_Private->m_MouseFilter = false;
        m_Private->m_ChangeFilter = true;
    } else {
        m_Private->m_ChangeFilter = false;
        m_Private->m_MouseFilter = false;
        m_Private->m_Image = image;
        int x(this->x());
        m_Private->m_X1 = 0;
        if (m_Private->m_Image.width() <= parentWidget()->width()) {
            m_Private->m_X1 = 0;
            m_Private->m_DeltaX = 0;
            x = (parentWidget()->width() - m_Private->m_Image.width()) * 0.5;
        } else {
            if (m_Private->m_Image.width() != m_Private->m_OldWidth) {
                x = (parentWidget()->width() - m_Private->m_Image.width()) * 0.5;
                float factorX = (m_Private->m_DeltaX / static_cast<float>(m_Private->m_OldWidth));
                if (m_Private->m_Image.width() < m_Private->m_OldWidth) {
                    m_Private->m_DeltaX = m_Private->m_DeltaX * (1 + factorX);
                    if (x <= 0) {
                        if (m_Private->m_DeltaX > 0) {
                            if (m_Private->m_DeltaX > qAbs(x)) {
                                m_Private->m_DeltaX = -x;
                            }
                        } else {
                            if (m_Private->m_DeltaX < x) {
                                m_Private->m_DeltaX = x;
                            }
                        }
                    }
                } else if (m_Private->m_Image.width() > m_Private->m_OldWidth) {
                    m_Private->m_DeltaX = factorX * m_Private->m_Image.width();
                }
            }
        }
        m_Private->m_OldWidth = m_Private->m_Image.width();

        int y(this->y());
        if (m_Private->m_Image.height() <= parentWidget()->height()) {
            m_Private->m_Y1 = 0;
            m_Private->m_DeltaY = 0;
            y = (parentWidget()->height() - m_Private->m_Image.height()) * 0.5;
        } else {
            if (m_Private->m_Image.height() != m_Private->m_OldHeight) {
                y = (parentWidget()->height() - m_Private->m_Image.height()) * 0.5;
                float factorY = (m_Private->m_DeltaY / static_cast<float>(m_Private->m_OldHeight));
                if (m_Private->m_Image.height() < m_Private->m_OldHeight) {
                    m_Private->m_DeltaY = m_Private->m_DeltaY * (1 + factorY);
                    if (y <= 0) {
                        if (m_Private->m_DeltaY >= 0) {
                            if (m_Private->m_DeltaY > qAbs(y)) {
                                m_Private->m_DeltaY = -y;
                            }
                        } else {
                            if (m_Private->m_DeltaY < y) {
                                m_Private->m_DeltaY = y;
                            }
                        }
                    }
                } else if (m_Private->m_Image.height() > m_Private->m_OldHeight) {
                    m_Private->m_DeltaY = factorY * m_Private->m_Image.height();
                }
            }
        }
        m_Private->m_OldHeight = m_Private->m_Image.height();
        setGeometry(x, y, m_Private->m_Image.width(), m_Private->m_Image.height());
        update();
    }
}

void PixmapWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    //    painter.drawPixmap(m_Private->m_DeltaX, m_Private->m_DeltaY, m_Private->m_Pixmap);
    painter.drawImage(m_Private->m_DeltaX, m_Private->m_DeltaY, m_Private->m_Image);
}

void PixmapWidget::mousePressEvent(QMouseEvent *event)
{
    if (!m_Private->m_ChangeFilter) {
        m_Private->m_X1 = event->pos().x();
        m_Private->m_Y1 = event->pos().y();
        stopTimer();
        QWidget::mousePressEvent(event);
    }
}

void PixmapWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (!m_Private->m_ChangeFilter) {
        qWarning() << __PRETTY_FUNCTION__ << __LINE__;
        if (x() < 0) {
            int x2  = event->pos().x();
            int deltaX = x2 - m_Private->m_X1;
            m_Private->m_X1 = event->pos().x();
            if ((deltaX + m_Private->m_DeltaX + x()) <= 0) {
                m_Private->m_DeltaX += deltaX;
            }
            if (m_Private->m_DeltaX > 0) {
                if (m_Private->m_DeltaX + x() >= 0) {
                    m_Private->m_DeltaX = -x();
                }
            } else {
                if (m_Private->m_DeltaX <= x()) {
                    m_Private->m_DeltaX = x();
                }
            }
        }
        if (y() < 0) {
            int y2  = event->pos().y();
            int deltaY = y2 - m_Private->m_Y1;
            m_Private->m_Y1 = event->pos().y();
            if ((deltaY + m_Private->m_DeltaY + y()) <= 0) {
                m_Private->m_DeltaY += deltaY;
            }
            if (m_Private->m_DeltaY > 0) {
                if (m_Private->m_DeltaY + y() >= 0) {
                    m_Private->m_DeltaY = -y();
                }
            } else {
                if (m_Private->m_DeltaY <= y()) {
                    m_Private->m_DeltaY = y();
                }
            }
        }
        update();
        QWidget::mouseMoveEvent(event);
    }
}

void PixmapWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (!m_Private->m_ChangeFilter) {
        startTimer();
        QWidget::mouseReleaseEvent(event);
    }
}

bool PixmapWidget::event(QEvent *event)
{
    switch (event->type()) {
    case QEvent::MouseButtonPress: {
        if (nullptr != parentWidget()) {
            bool condition1(m_Private->m_Pixmap.width() < parentWidget()->width());
            bool condition2(m_Private->m_Pixmap.height() < parentWidget()->height());
            if ((condition1 && condition2)) {

            } else {
                QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
                if (nullptr != mouseEvent) {
                    mousePressEvent(mouseEvent);
                }
                return true;
            }
        }
        break;
    }
    case QEvent::MouseButtonRelease: {
        if (m_Private->m_MouseFilter) {
            m_Private->m_MouseFilter = false;
            return true;
        }
        break;
    }
    case QEvent::MouseMove: {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent *>(event);
        mouseMoveEvent(mouseEvent);
        event->accept();
        m_Private->m_MouseFilter = true;
        return true;
        break;
    }
    default: {
        break;
    }
    }
    return QWidget::event(event);
}

PixmapWidgetPrivate::PixmapWidgetPrivate(PixmapWidget *parent)
    : m_Parent(parent)
{
    connectAllSlots();
}

PixmapWidgetPrivate::~PixmapWidgetPrivate()
{
}

void PixmapWidgetPrivate::connectAllSlots()
{
}
