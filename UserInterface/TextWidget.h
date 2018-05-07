#ifndef TEXTWIDGET_H
#define TEXTWIDGET_H

#include <QWidget>
#include <QScopedPointer>

class TextWidgetPrivate;
class TextWidget : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(TextWidget)
public:
    enum Type {
        T_Translate = 0,
        T_NoTranslate = 1
    };
    explicit TextWidget(QWidget *parent = NULL);
    ~TextWidget();
    void setText(const QString &text);
    void setLanguageType(const TextWidget::Type type);
    void setAlignmentFlag(const int flags);
    void setFontPointSize(const int pointSize);
    void setAnimation(const bool flag);
    void setTextElideMode(const Qt::TextElideMode mode);
protected:
    void paintEvent(QPaintEvent *event);
private slots:
    void onTimeout();
private:
    QScopedPointer<TextWidgetPrivate> m_Private;
};

#endif // TEXTWIDGET_H
