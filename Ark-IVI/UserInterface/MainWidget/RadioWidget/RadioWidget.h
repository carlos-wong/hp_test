#ifndef RADIOWIDGET_H
#define RADIOWIDGET_H

#include "BusinessLogic/Widget.h"
#include "BusinessLogic/Radio.h"
#include <QWidget>
#include <QPushButton>

class TestControl : public QWidget
{
    Q_OBJECT
public:
    explicit TestControl(QWidget* parent);
    ~TestControl();
    void setValue(const unsigned char value);
    void setTitle(const QString& title);
signals:
    void minus();
    void plus();
public:
    QPushButton* m_Title;
    QPushButton* m_Minus;
    QPushButton* m_Value;
    QPushButton* m_Plus;
};

class RadioWidgetPrivate;
class RadioWidget
        : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(RadioWidget)
public:
    explicit RadioWidget(QWidget *parent = NULL);
    ~RadioWidget();
    void makeSureComplete();
protected:
    void resizeEvent(QResizeEvent* event);
    void mousePressEvent(QMouseEvent* event);
protected slots:
    void onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status);
    void onRadioSmeter(const QByteArray& byteArray);
private slots:
    void onMinus();
    void onPlus();
private:
    Q_DECLARE_PRIVATE(RadioWidget)
    RadioWidgetPrivate* const d_ptr;
};

#endif // RADIOWIDGET_H
