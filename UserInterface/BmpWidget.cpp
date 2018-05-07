#include "BmpWidget.h"
//#include "UserInterfaceUtility.h"
#include <QPainter>
#include <QFile>
#include <QScopedPointer>

class BmpWidgetPrivate
{
    Q_DISABLE_COPY(BmpWidgetPrivate)
public:
    explicit BmpWidgetPrivate(BmpWidget* parent);
    ~BmpWidgetPrivate();
    QScopedPointer<QPixmap> m_BackgroundPixmap;
    bool m_AutoFillBackground;
    bool m_Adapted;
    void adaptImage();
private:
    BmpWidget* m_Parent;
};

BmpWidget::BmpWidget(QWidget *parent)
    : QWidget(parent)
    , m_Private(new BmpWidgetPrivate(this))
{
}

BmpWidget::~BmpWidget()
{
}

void BmpWidget::setBackgroundBmpPath(const QString &path)
{
    if (QFile::exists(path)) {
        m_Private->m_BackgroundPixmap.reset(new QPixmap(path));
        if (isVisible()) {
            update();
        }
    }
}

void BmpWidget::setImageAutoFillBackground(bool flag)
{
    m_Private->m_AutoFillBackground = flag;
}

void BmpWidget::setWidgetAdaptToImage(bool flag)
{
    m_Private->m_Adapted = flag;
    if (m_Private->m_Adapted) {
        m_Private->adaptImage();
    }
}

void BmpWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    if (!m_Private->m_BackgroundPixmap.isNull()) {
        if (!m_Private->m_AutoFillBackground) {
            int x = 0;
            int y = 0;
            if (!m_Private->m_Adapted) {
                x = (width() - m_Private->m_BackgroundPixmap->width()) >> 1;
                y = (height() - m_Private->m_BackgroundPixmap->height()) >> 1;
            }
            painter.drawPixmap(x, y, *m_Private->m_BackgroundPixmap);
        } else {
            painter.drawPixmap(this->rect(), *m_Private->m_BackgroundPixmap);
        }
    }
    QWidget::paintEvent(event);
}

void BmpWidget::resizeEvent(QResizeEvent *)
{
    if (m_Private->m_Adapted) {
        m_Private->adaptImage();
    }
}

BmpWidgetPrivate::BmpWidgetPrivate(BmpWidget *parent)
    : m_Parent(parent)
{
    m_AutoFillBackground = false;
    m_Adapted = false;
}

BmpWidgetPrivate::~BmpWidgetPrivate()
{
}

void BmpWidgetPrivate::adaptImage()
{
    if (!m_BackgroundPixmap.isNull()) {
        QSize imageSize = m_BackgroundPixmap->size();
        if (imageSize != m_Parent->size()) {
            m_Parent->resize(imageSize);
        }
    }
}
