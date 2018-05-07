#include "TextWidget.h"
#include <QPainter>
#include <QTimer>
#include <QDebug>
#include <QtGlobal>
using namespace Qt;
class TextWidgetPrivate
{
    Q_DISABLE_COPY(TextWidgetPrivate)
public:
    explicit TextWidgetPrivate(TextWidget* parent);
    ~TextWidgetPrivate();
    QString m_Text;
    int m_Flags;
    QFont m_Font;
    TextWidget::Type m_LanguageType;
    bool m_Animation;
    int m_TextWidth;
    int m_XPos;
    QTimer* m_Timer;
    TextElideMode m_TextElideMode;
private:
    TextWidget* m_Parent;
};

TextWidget::TextWidget(QWidget *parent)
    : QWidget(parent)
    , m_Private(new TextWidgetPrivate(this))
{
}

TextWidget::~TextWidget()
{
}

void TextWidget::setText(const QString &text)
{
    m_Private->m_Text = text;
    m_Private->m_TextWidth = fontMetrics().width(m_Private->m_Text);
    if (NULL != m_Private->m_Timer) {
        if (m_Private->m_TextWidth > width()) {
            m_Private->m_Timer->setSingleShot(false);
            m_Private->m_Timer->start(33);
            m_Private->m_XPos = 0;
        } else {
            m_Private->m_Timer->stop();
            m_Private->m_XPos = 0;
        }
    }
    if (isVisible()) {
        update();
    }
}

void TextWidget::setLanguageType(const TextWidget::Type type)
{
    m_Private->m_LanguageType = type;
    if (isVisible()) {
        update();
    }
}

void TextWidget::setAlignmentFlag(const int flags)
{
    m_Private->m_Flags = flags;
    if (isVisible()) {
        update();
    }
}

void TextWidget::setFontPointSize(const int pointSize)
{
    m_Private->m_Font.setPointSize(pointSize);
    setFont(m_Private->m_Font);
    if (isVisible()) {
        update();
    }
}

void TextWidget::setAnimation(const bool flag)
{
    m_Private->m_Animation = flag;
    if (NULL == m_Private->m_Timer) {
        m_Private->m_Timer = new QTimer(this);
        Qt::ConnectionType type = static_cast<Qt::ConnectionType>(Qt::AutoConnection | Qt::UniqueConnection);
        QObject::connect(m_Private->m_Timer, SIGNAL(timeout()),
                         this,               SLOT(onTimeout()),
                         type);
    }
}

void TextWidget::setTextElideMode(const Qt::TextElideMode mode)
{
    m_Private->m_TextElideMode = mode;
    if (isVisible()) {
        update();
    }
}

void TextWidget::paintEvent(QPaintEvent *event)
{
    if (!m_Private->m_Text.isEmpty()) {
        QPainter painter(this);
        painter.setPen(QPen(Qt::white));
        if (TextWidget::T_Translate == m_Private->m_LanguageType) {
            painter.drawText(rect(), m_Private->m_Flags, QObject::tr(m_Private->m_Text.toLocal8Bit().constData()));
        } else {
            if (NULL != m_Private->m_Timer) {
                if (m_Private->m_TextWidth > width()) {
                    painter.drawText(QRect(m_Private->m_XPos, 0, m_Private->m_TextWidth, height()), m_Private->m_Text);
                } else {
                    painter.drawText(rect(), m_Private->m_Flags, m_Private->m_Text);
                }
            } else {
                QFontMetrics fontMetrics(painter.font());
                QString text = fontMetrics.elidedText(m_Private->m_Text, m_Private->m_TextElideMode, width());
                painter.drawText(rect(), m_Private->m_Flags, text);
            }
        }
    }
    QWidget::paintEvent(event);
}

void TextWidget::onTimeout()
{
    --m_Private->m_XPos;
    if (abs(m_Private->m_XPos) > m_Private->m_TextWidth) {
        m_Private->m_XPos = width();
    }
    if (isVisible()) {
        update();
    }
}

TextWidgetPrivate::TextWidgetPrivate(TextWidget* parent)
    : m_Parent(parent)
{
    m_Flags = Qt::AlignCenter;
    m_LanguageType = TextWidget::T_Translate;
    m_Animation = false;
    m_TextWidth = 0;
    m_XPos = 0;
    m_Timer = NULL;
    m_TextElideMode = ElideRight;
    m_Parent->setFont(m_Font);
}

TextWidgetPrivate::~TextWidgetPrivate()
{
}
