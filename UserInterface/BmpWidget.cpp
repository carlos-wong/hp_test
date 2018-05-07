#include "BmpWidget.h"
#include "UserInterfaceUtility.h"
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

void BmpWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    if (!m_Private->m_BackgroundPixmap.isNull()) {
        int x = (width() - m_Private->m_BackgroundPixmap->width()) >> 1;
        int y = (height() - m_Private->m_BackgroundPixmap->height()) >> 1;
        painter.drawPixmap(x, y, *m_Private->m_BackgroundPixmap);
    }
    QWidget::paintEvent(event);
}

BmpWidgetPrivate::BmpWidgetPrivate(BmpWidget *parent)
    : m_Parent(parent)
{
}

BmpWidgetPrivate::~BmpWidgetPrivate()
{
}
