#include "PixmapWidget.h"
#include "UserInterfaceUtility.h"
#include "BusinessLogic/Widget.h"
#include <QPainter>
#include <QMouseEvent>
#include <QImage>
#include <QtGlobal>

static const unsigned short int max_width = 4000;
static const unsigned short int max_height = 2400;
static const unsigned short int min_width = 160;
static const unsigned short int min_height = 96;
static const float scale_step = 0.5f;

class PixmapWidgetPrivate
{
    Q_DISABLE_COPY(PixmapWidgetPrivate)
public:
    explicit PixmapWidgetPrivate(PixmapWidget* parent);
    ~PixmapWidgetPrivate();
    void connectAllSlots();
    void rotate(const unsigned short int index);
    QImage m_Image;
    unsigned short int  m_TranslateX;
    unsigned short int m_TranslateY;
    int m_X1;
    int m_Y1;
    int m_DX;
    int m_DY;
    bool m_MouseFilter;
    bool m_ChangeFilter;
    bool m_GIF;
    unsigned short int m_DW;
    unsigned short int m_DH;
    unsigned short int m_SW;
    unsigned short int m_SH;
    unsigned short int m_Rotate;
    float m_DAspectRatio;
    float m_SAspectRatio;
    float m_Scale;
private:
    PixmapWidget* m_Parent;
};

PixmapWidget::PixmapWidget(QWidget *parent)
    : QWidget(parent)
    , m_Private(new PixmapWidgetPrivate(this))
{
    g_Widget->geometryFit(0, 0, 800, 480, this);
}

PixmapWidget::~PixmapWidget()
{
}

void PixmapWidget::onImageChange(const QImage &image, const bool change, const bool gif)
{
    m_Private->m_Image = image;
    m_Private->m_ChangeFilter = change;
    m_Private->m_GIF = gif;
    if (change) {
        m_Private->m_DAspectRatio = (float)m_Private->m_Image.width() / m_Private->m_Image.height();
        m_Private->m_SAspectRatio = static_cast<float>(width()) / height();
        m_Private->m_Scale = 1.0f;
        m_Private->m_Rotate = 0;
    }
    m_Private->rotate(m_Private->m_Rotate);
}

void PixmapWidget::rotate()
{
    if (!m_Private->m_Image.isNull()) {
        m_Private->m_Scale = 1.0f;
        m_Private->m_DW = m_Private->m_SW * m_Private->m_Scale;
        m_Private->m_DH = m_Private->m_SH * m_Private->m_Scale;
        ++m_Private->m_Rotate;
        m_Private->m_Rotate %= 4;
        m_Private->m_ChangeFilter = true;
        m_Private->rotate(m_Private->m_Rotate);
        if (m_Private->m_GIF) {
            m_Private->m_ChangeFilter = false;
        }
    }
}

void PixmapWidget::scale()
{
    if (!m_Private->m_Image.isNull()) {
        m_Private->m_DW = m_Private->m_SW * m_Private->m_Scale;
        m_Private->m_DH = m_Private->m_SH * m_Private->m_Scale;
        switch (m_Private->m_Rotate) {
        case 1: {
            m_Private->m_DX = (height() - m_Private->m_DW) >> 1;
            m_Private->m_DY = (width() - m_Private->m_DH) >> 1;
            break;
        }
        case 2: {
            m_Private->m_DX = (width() - m_Private->m_DW) >> 1;
            m_Private->m_DY = (height() - m_Private->m_DH) >> 1;
            break;
        }
        case 3: {
            m_Private->m_DX = (height() - m_Private->m_DW) >> 1;
            m_Private->m_DY = (width() - m_Private->m_DH) >> 1;
            break;
        }
        default: {
            m_Private->m_DX = (width() - m_Private->m_DW) >> 1;
            m_Private->m_DY = (height() - m_Private->m_DH) >> 1;
            break;
        }
        }
    }
    update();
}

void PixmapWidget::zoomOut()
{
    if (!m_Private->m_Image.isNull()) {
        float tmpscale = m_Private->m_Scale + scale_step;
        unsigned short int w = m_Private->m_SW * tmpscale;
        unsigned short int h = m_Private->m_SH * tmpscale;
        if (((m_Private->m_Image.width() << 2) > m_Private->m_DW)
                && ((m_Private->m_Image.height() << 2) > m_Private->m_DH)) {
            if ((w < max_width) && (h < max_height)) {
                m_Private->m_Scale += scale_step;
                scale();
            }
        }
    }
}

void PixmapWidget::zoomIn()
{
    if (!m_Private->m_Image.isNull()) {
        float tmpscale = m_Private->m_Scale - scale_step;
        if (((m_Private->m_DW * tmpscale) > min_width)
                && ((m_Private->m_DH * tmpscale) > min_height)) {
            m_Private->m_Scale -= scale_step;
            scale();
        }
    }
}

void PixmapWidget::paintEvent(QPaintEvent *event)
{
    if (!m_Private->m_Image.isNull()) {
        QPainter painter(this);
        painter.scale(m_Private->m_Scale, m_Private->m_Scale);
        painter.translate(m_Private->m_TranslateX / m_Private->m_Scale, m_Private->m_TranslateY / m_Private->m_Scale);
        painter.rotate(90 * m_Private->m_Rotate);
        //        qDebug() << __PRETTY_FUNCTION__ << "m_Private->m_DX" << m_Private->m_DX;
        //        qDebug() << __PRETTY_FUNCTION__ << "m_Private->m_DY" << m_Private->m_DY;
        //        qDebug() << __PRETTY_FUNCTION__ << "m_Private->m_DW" << m_Private->m_DW;
        //        qDebug() << __PRETTY_FUNCTION__ << "m_Private->m_DH" << m_Private->m_DH;
        //        qDebug() << __PRETTY_FUNCTION__ << "m_Private->m_Scale" << m_Private->m_Scale;
        painter.drawImage(QRect(m_Private->m_DX / m_Private->m_Scale,
                                m_Private->m_DY / m_Private->m_Scale,
                                m_Private->m_DW / m_Private->m_Scale,
                                m_Private->m_DH / m_Private->m_Scale),
                          m_Private->m_Image);
        painter.translate(-m_Private->m_TranslateX / m_Private->m_Scale, -m_Private->m_TranslateY / m_Private->m_Scale);
    }
}

void PixmapWidget::mousePressEvent(QMouseEvent *event)
{
    if (!m_Private->m_ChangeFilter) {
        m_Private->m_X1 = event->pos().x();
        m_Private->m_Y1 = event->pos().y();
        stopTimer();
    }
    QWidget::mousePressEvent(event);
}

void PixmapWidget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->pos().x() - m_Private->m_X1;
    m_Private->m_X1 = event->pos().x();
    int dy = event->pos().y() - m_Private->m_Y1;
    m_Private->m_Y1 = event->pos().y();
    short int flag = 1;
    switch (m_Private->m_Rotate) {
    case 3: {
        flag = -1;
    }
    case 1: {
        if (m_Private->m_DX >= (height() - m_Private->m_DW)) {
            if ((m_Private->m_DX + (dy * flag)) > 0) {
                m_Private->m_DX = 0;
            } else {
                m_Private->m_DX += (dy * flag);
                if (m_Private->m_DX < (height() - m_Private->m_DW)) {
                    m_Private->m_DX = (height() - m_Private->m_DW);
                }
            }
        }
        if (m_Private->m_DY >= (width() - m_Private->m_DH)) {
            if ((m_Private->m_DY - (dx * flag)) > 0) {
                m_Private->m_DY = 0;
            } else {
                m_Private->m_DY -= (dx * flag);
                if (m_Private->m_DY < (width() - m_Private->m_DH)) {
                    m_Private->m_DY = (width() - m_Private->m_DH);
                }
            }
        }
        break;
    }
    case 2: {
        flag = -1;
    }
    default: {
        if (m_Private->m_DX >= (width() - m_Private->m_DW)) {
            if ((m_Private->m_DX + (dx * flag)) > 0) {
                m_Private->m_DX = 0;
            } else {
                m_Private->m_DX += (dx * flag);
                if (m_Private->m_DX < (width() - m_Private->m_DW)) {
                    m_Private->m_DX = (width() - m_Private->m_DW);
                }
            }
        }
        if (m_Private->m_DY >= (height() - m_Private->m_DH)) {
            if ((m_Private->m_DY + (dy * flag)) > 0) {
                m_Private->m_DY = 0;
            } else {
                m_Private->m_DY += (dy * flag);
                if (m_Private->m_DY < (height() - m_Private->m_DH)) {
                    m_Private->m_DY = (height() - m_Private->m_DH);
                }
            }
        }
        break;
    }
    }
    update();
    QWidget::mousePressEvent(event);
}

void PixmapWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (!m_Private->m_ChangeFilter) {
        startTimer();
    }
    QWidget::mouseReleaseEvent(event);
}

bool PixmapWidget::event(QEvent *event)
{
    switch (event->type()) {
    case QEvent::MouseButtonPress: {
        if (NULL != parentWidget()) {
            bool condition1(m_Private->m_Image.width() < parentWidget()->width());
            bool condition2(m_Private->m_Image.height() < parentWidget()->height());
            if ((condition1 && condition2)) {

            } else {
                QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
                if (NULL != mouseEvent) {
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
    m_TranslateX = 0;
    m_TranslateY = 0;
    m_X1 = 0;
    m_Y1 = 0;
    m_DX = 0;
    m_DY = 0;
    m_MouseFilter = false;
    m_ChangeFilter = false;
    m_GIF = false;
    m_DW = 0;
    m_DH = 0;
    m_SW = 0;
    m_SH = 0;
    m_Rotate = 0;
    m_DAspectRatio = 1.0f;
    m_SAspectRatio;
    m_Scale = 1.0f;
    connectAllSlots();
}

PixmapWidgetPrivate::~PixmapWidgetPrivate()
{
}

void PixmapWidgetPrivate::connectAllSlots()
{
}

void PixmapWidgetPrivate::rotate(const unsigned short index)
{
    if (m_ChangeFilter) {
        switch (index) {
        case 1:
        case 3: {
            unsigned short int widgetWidth = m_Parent->height() * m_Scale;
            unsigned short int widgetHeight = m_Parent->width() * m_Scale;
            unsigned short int drawWidth;
            unsigned short int drawHeight;
            if (1 == m_Rotate) {
                m_TranslateX = m_Parent->width();
                m_TranslateY = 0;
            } else {
                m_TranslateX = 0;
                m_TranslateY = m_Parent->height();
            }
            if (m_Image.width() <= widgetWidth) {
                if (m_Image.height() <= widgetHeight) {
                    drawWidth = m_Image.width();
                    drawHeight = m_Image.height();
                } else {
                    drawHeight = widgetHeight;
                    drawWidth = m_DAspectRatio * drawHeight;
                }
            } else {
                if (m_Image.height() <= widgetHeight) {
                    drawWidth = widgetWidth;
                    drawHeight = m_DW / m_DAspectRatio;
                } else {
                    drawWidth = widgetWidth;
                    drawHeight = drawWidth / m_DAspectRatio;
                }
            }
            m_DW = drawWidth;
            m_DH = drawHeight;
            m_DX = (widgetWidth - m_DW) >> 1;
            m_DY = (widgetHeight - m_DH) >> 1;
            break;
        }
        default: {
            unsigned short int widgetWidth = m_Parent->width() * m_Scale;
            unsigned short int widgetHeight = m_Parent->height() * m_Scale;
            unsigned short int drawWidth;
            unsigned short int drawHeight;
            if (0 == m_Rotate) {
                m_TranslateX = 0;
                m_TranslateY = 0;
            } else {
                m_TranslateX = m_Parent->width();
                m_TranslateY = m_Parent->height();
            }
            if (m_Image.width() <= widgetWidth) {
                if (m_Image.height() <= widgetHeight) {
                    drawWidth = m_Image.width();
                    drawHeight = m_Image.height();
                } else {
                    drawHeight = widgetHeight;
                    drawWidth = m_DAspectRatio * drawHeight;
                }
            } else {
                if (m_Image.height() <= widgetHeight) {
                    drawWidth = widgetWidth;
                    drawHeight = m_DW / m_DAspectRatio;
                } else {
                    if (m_DAspectRatio > m_SAspectRatio) {
                        drawWidth = widgetWidth;
                        drawHeight = drawWidth / m_DAspectRatio;
                    } else {
                        drawHeight = widgetHeight;
                        drawWidth = m_DAspectRatio * drawHeight;
                    }
                }
            }
            m_DW = drawWidth;
            m_DH = drawHeight;
            m_SW = m_DW;
            m_SH = m_DH;
            m_DX = (widgetWidth - m_DW) >> 1;
            m_DY = (widgetHeight - m_DH) >> 1;
            break;
        }
        }
    }
    m_Parent->update();
}
